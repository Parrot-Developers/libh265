/**
 * Copyright (c) 2019 Parrot Drones SAS
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Parrot Drones SAS Company nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE PARROT DRONES SAS COMPANY BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "h265_priv.h"


struct h265_reader {
	struct h265_ctx_cbs cbs;
	void *userdata;
	int stop;
	struct h265_ctx *ctx;
	uint32_t flags;
};


#define H265_SYNTAX_OP_NAME read
#define H265_SYNTAX_OP_KIND H265_SYNTAX_OP_KIND_READ

#define H265_BITS(_f, _n) H265_READ_BITS(_f, _n)
#define H265_BITS_U(_f, _n) H265_READ_BITS_U(_f, _n)
#define H265_BITS_I(_f, _n) H265_READ_BITS_I(_f, _n)
#define H265_BITS_UE(_f) H265_READ_BITS_UE(_f)
#define H265_BITS_SE(_f) H265_READ_BITS_SE(_f)

#define H265_BITS_RBSP_TRAILING()                                              \
	do {                                                                   \
		int _res = h265_bs_read_rbsp_trailing_bits(bs);                \
		ULOG_ERRNO_RETURN_ERR_IF(_res < 0, -_res);                     \
	} while (0)

#include "h265_syntax.h"


static int h265_reader_init(const struct h265_ctx_cbs *cbs,
			    void *userdata,
			    struct h265_reader *reader)
{
	ULOG_ERRNO_RETURN_ERR_IF(cbs == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(reader == NULL, EINVAL);

	reader->cbs = *cbs;
	reader->userdata = userdata;
	int res = h265_ctx_new(&reader->ctx);
	if (res < 0)
		return res;

	return 0;
}


int h265_reader_new(const struct h265_ctx_cbs *cbs,
		    void *userdata,
		    struct h265_reader **ret_obj)
{
	ULOG_ERRNO_RETURN_ERR_IF(ret_obj == NULL, EINVAL);

	struct h265_reader *reader = calloc(1, sizeof(**ret_obj));
	if (reader == NULL)
		return -ENOMEM;

	int res = h265_reader_init(cbs, userdata, reader);

	if (res < 0) {
		h265_reader_destroy(reader);
		return res;
	}

	*ret_obj = reader;

	return 0;
}


int h265_reader_destroy(struct h265_reader *reader)
{
	if (reader == NULL)
		return 0;

	int res = h265_ctx_destroy(reader->ctx);
	free(reader);
	return res;
}


int h265_reader_stop(struct h265_reader *reader)
{
	reader->stop = 1;
	return 0;
}


struct h265_ctx *h265_reader_get_ctx(struct h265_reader *reader)
{
	return reader == NULL ? NULL : reader->ctx;
}


int h265_reader_parse(struct h265_reader *reader,
		      uint32_t flags,
		      const uint8_t *buf,
		      size_t len,
		      size_t *off)
{
	int res = 0;
	size_t start = 0;
	size_t end = 0;
	*off = 0;

	ULOG_ERRNO_RETURN_ERR_IF(reader == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(buf == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(off == NULL, EINVAL);

	reader->stop = 0;

	while (*off < len && !reader->stop) {
		res = h265_find_nalu(buf + *off, len - *off, &start, &end);
		if (res < 0 && res != -EAGAIN)
			break;

		h265_reader_parse_nalu(
			reader, flags, buf + *off + start, end - start);

		*off += end;
	}

	return 0;
}


int h265_reader_parse_nalu(struct h265_reader *reader,
			   uint32_t flags,
			   const uint8_t *buf,
			   size_t len)
{
	int res = 0;
	struct h265_bitstream bs;

	ULOG_ERRNO_RETURN_ERR_IF(reader == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(buf == NULL, EINVAL);

	reader->stop = 0;
	reader->flags = flags;
	h265_bs_cinit(&bs, buf, len, 1);
	bs.priv = reader;
	res = _h265_read_nalu(&bs, reader->ctx, &reader->cbs, reader->userdata);
	h265_bs_clear(&bs);

	return res;
}


int h265_parse_vps(const uint8_t *buf, size_t len, struct h265_vps *vps)
{
	struct h265_bitstream bs;
	h265_bs_cinit(&bs, buf, len, 1);

	ULOG_ERRNO_RETURN_ERR_IF(buf == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(vps == NULL, EINVAL);

	struct h265_nalu_header nh = {0};

	int res = _h265_read_nalu_header(&bs, &nh);
	if (res < 0)
		goto out;
	if (nh.nal_unit_type != H265_NALU_TYPE_VPS_NUT) {
		res = -EIO;
		ULOGE("invalid nalu type: %u (%u)",
		      nh.nal_unit_type,
		      H265_NALU_TYPE_VPS_NUT);
		goto out;
	}

	res = _h265_read_vps(&bs, vps);

out:
	h265_bs_clear(&bs);
	return res;
}


int h265_parse_sps(const uint8_t *buf, size_t len, struct h265_sps *sps)
{
	struct h265_bitstream bs;
	h265_bs_cinit(&bs, buf, len, 1);

	ULOG_ERRNO_RETURN_ERR_IF(buf == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(sps == NULL, EINVAL);

	struct h265_nalu_header nh = {0};

	int res = _h265_read_nalu_header(&bs, &nh);
	if (res < 0)
		goto out;
	if (nh.nal_unit_type != H265_NALU_TYPE_SPS_NUT) {
		res = -EIO;
		ULOGE("invalid nalu type: %u (%u)",
		      nh.nal_unit_type,
		      H265_NALU_TYPE_SPS_NUT);
		goto out;
	}

	res = _h265_read_sps(&bs, sps);

out:
	h265_bs_clear(&bs);
	return res;
}


int h265_parse_pps(const uint8_t *buf, size_t len, struct h265_pps *pps)
{
	struct h265_bitstream bs;
	h265_bs_cinit(&bs, buf, len, 1);

	ULOG_ERRNO_RETURN_ERR_IF(buf == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(pps == NULL, EINVAL);

	struct h265_nalu_header nh = {0};

	int res = _h265_read_nalu_header(&bs, &nh);
	if (res < 0)
		goto out;
	if (nh.nal_unit_type != H265_NALU_TYPE_PPS_NUT) {
		res = -EIO;
		ULOGE("invalid nalu type: %u (%u)",
		      nh.nal_unit_type,
		      H265_NALU_TYPE_PPS_NUT);
		goto out;
	}

	res = _h265_read_pps(&bs, pps);

out:
	h265_bs_clear(&bs);
	return res;
}
