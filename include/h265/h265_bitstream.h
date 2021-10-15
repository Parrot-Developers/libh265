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

#ifndef _H265_BITSTREAM_H_
#define _H265_BITSTREAM_H_


struct h265_bitstream {
	union {
		/* Data pointer (const) */
		const uint8_t *cdata;

		/* Data pointer */
		uint8_t *data;
	};

	/* Data length */
	size_t len;

	/* Offset in data */
	size_t off;

	/* Partial read/write byte */
	uint8_t cache;

	/* Number of bits in cache */
	uint8_t cachebits;

	/* Enable emulation prevention */
	int emulation_prevention;

	/* Dynamic */
	int dynamic;

	/* Private data */
	void *priv;
};


/**
 * Get the first NAL unit in buf.
 *
 * This function returns a slice of the data contained in the next NAL unit in
 * buf. If buf points to somewhere inside a NAL unit, the next NAL unit in the
 * bytestream is returned, if any.
 *
 * @param[in] buf,len Bytestream buffer
 * @param[out] start An offset such that buf[start] is the first byte of the
 * returned NAL unit
 * @param[out] end An offset such that buf[end] is one byte past the end of
 * the returned NAL unit
 *
 * @return 0 on success, negative errno value in case of error
 */
H265_API
int h265_find_nalu(const uint8_t *buf, size_t len, size_t *start, size_t *end);


/**
 * Write bits to the stream.
 *
 * @param bs Bitstream handle
 * @param v Bit mask. The n least significant bits of the mask are inserted
 * in most significant to least significant order
 * @param n Number of bits to extract from v into the stream
 *
 * @return Number of bits written on success, negative errno value in case of
 * error
 */
H265_API
int h265_bs_write_bits(struct h265_bitstream *bs, uint32_t v, uint32_t n);


H265_API
int h265_bs_read_bits_ue(struct h265_bitstream *bs, uint32_t *v);


H265_API
int h265_bs_write_bits_ue(struct h265_bitstream *bs, uint32_t v);


H265_API
int h265_bs_read_bits_ff_coded(struct h265_bitstream *bs, uint32_t *v);


H265_API
int h265_bs_write_bits_ff_coded(struct h265_bitstream *bs, uint32_t v);


/**
 * 7.2 Specification of syntax functions and descriptors.
 */
H265_API
int h265_bs_more_rbsp_data(const struct h265_bitstream *bs);


/**
 * 7.3.2.11 RBSP trailing bits syntax.
 */
H265_API
int h265_bs_read_rbsp_trailing_bits(struct h265_bitstream *bs);


H265_API
int h265_bs_write_rbsp_trailing_bits(struct h265_bitstream *bs);


H265_API
int h265_bs_read_raw_bytes(struct h265_bitstream *bs, uint8_t *buf, size_t len);


H265_API
int h265_bs_write_raw_bytes(struct h265_bitstream *bs,
			    const uint8_t *buf,
			    size_t len);


/**
 * Take ownership of a bitstream's buffer.
 *
 * This function will return an error if the stream is not currently
 * byte-aligned or if the bitstream does not own its data.
 *
 * @param[in] bs Bitstream instance handle
 * @param[out] buf Buffer data pointer
 * @param[out] len Buffer length
 *
 * @return 0 on success, negative error value in case of error
 */
H265_API
int h265_bs_acquire_buf(struct h265_bitstream *bs, uint8_t **buf, size_t *len);


static inline void h265_bs_cinit(struct h265_bitstream *bs,
				 const uint8_t *buf,
				 size_t len,
				 int emulation_prevention)
{
	memset(bs, 0, sizeof(*bs));
	bs->cdata = buf;
	bs->len = len;
	bs->emulation_prevention = emulation_prevention;
}


/**
 * Initialize a bitstream.
 *
 * @param[in] bs Uninitialized bitstream
 * @param[in] buf Pointer to an array of bytes, or NULL
 * @param[in] len Length of the array pointed to by buf, or 0
 * if buf is NULL
 * @param[in] emulation_prevention Whether to skip emulation prevention
 * bytes
 */
static inline void h265_bs_init(struct h265_bitstream *bs,
				uint8_t *buf,
				size_t len,
				int emulation_prevention)
{
	memset(bs, 0, sizeof(*bs));
	bs->data = buf;
	bs->len = len;
	bs->dynamic = (buf == NULL && len == 0);
	bs->emulation_prevention = emulation_prevention;
}


static inline void h265_bs_clear(struct h265_bitstream *bs)
{
	if (bs->dynamic)
		free(bs->data);
	memset(bs, 0, sizeof(*bs));
}


/**
 * 7.2 Query whether the current position in the bitstream is on a byte
 * boundary.
 *
 * @param bs Bitstream instance handle
 *
 * @return A non-zero value if the current position is byte-aligned, 0
 * otherwise
 */
static inline int h265_bs_byte_aligned(const struct h265_bitstream *bs)
{
	return bs->cachebits % 8 == 0;
}


/**
 * Query whether end of stream was reached.
 *
 * @param bs Bitstream instance handle
 *
 * @return 1 if end of stream was reached, 0 otherwise
 */
static inline int h265_bs_eos(const struct h265_bitstream *bs)
{
	return bs->off >= bs->len && bs->cachebits == 0;
}


/**
 * Get the number of bits remaining in the stream.
 *
 * @param bs Bitstream instance handle
 *
 * @return Number of bits left in the stream
 */
static inline size_t h265_bs_rem_raw_bits(const struct h265_bitstream *bs)
{
	return (bs->len - bs->off) * 8 + bs->cachebits;
}


/**
 * Fetch a byte from the stream and cache its bits.
 *
 * This function takes emulation prevention into account and skips escape bytes.
 *
 * @param bs Bitstream instance handle
 *
 * @return 0 on success, -EIO if end of stream is reached
 */
static inline int h265_bs_fetch(struct h265_bitstream *bs)
{
	/* Detect 0x00 0x00 0x03 sequence in the stream */
	if (bs->emulation_prevention && bs->off >= 2 &&
	    bs->cdata[bs->off - 2] == 0x00 && bs->cdata[bs->off - 1] == 0x00 &&
	    bs->cdata[bs->off] == 0x03) {
		if (bs->off + 1 >= bs->len)
			return -EIO;
		/* Skip escape byte */
		bs->cache = bs->cdata[bs->off + 1];
		bs->cachebits = 8;
		bs->off += 2;
		return 0;
	} else if (bs->off < bs->len) {
		bs->cache = bs->cdata[bs->off];
		bs->cachebits = 8;
		bs->off++;
		return 0;
	} else {
		/* End of stream reached */
		return -EIO;
	}
}


/**
 * 7.2 Read n bits from the stream.
 *
 * This function reads at most n bits from the stream and stores them in v.
 * If n is greater than 32, v will be filled with the last 32 bits read.
 *
 * @param[in] bs Bitstream handle
 * @param[out] v Bit mask
 * @param[in] n	Number of bits to extract from the stream into v
 *
 * @return Number of bits written on success, negative errno value in case of
 * error
 */
static inline int
h265_bs_read_bits(struct h265_bitstream *bs, uint32_t *v, uint32_t n)
{
	int res = 0;
	uint32_t bits = 0;
	uint32_t mask = 0;
	uint32_t part = 0;

	*v = 0;
	while (n > 0) {
		/* Fetch data if needed */
		if (bs->cachebits == 0 && h265_bs_fetch(bs) < 0)
			return -EIO;

		/* Read as many bits from cache */
		bits = n < bs->cachebits ? n : bs->cachebits;
		mask = (1 << bits) - 1;
		part = (bs->cache >> (bs->cachebits - bits)) & mask;
		*v = (*v << bits) | part;
		n -= bits;
		bs->cachebits -= bits;
		res += bits;
	}

	return res;
}


/**
 * 7.2 Read an unsigned integer using n bits.
 */
static inline int
h265_bs_read_bits_u(struct h265_bitstream *bs, uint32_t *v, uint32_t n)
{
	return h265_bs_read_bits(bs, v, n);
}


static inline int
h265_bs_write_bits_u(struct h265_bitstream *bs, uint32_t v, uint32_t n)
{
	return h265_bs_write_bits(bs, v, n);
}


/**
 * 7.2 Read a signed integer using n bits.
 */
static inline int
h265_bs_read_bits_i(struct h265_bitstream *bs, int32_t *v, uint32_t n)
{
	int res = 0;
	uint32_t u32 = 0;

	res = h265_bs_read_bits(bs, &u32, n);
	if (res >= 0) {
		/* Sign extend result */
		if ((u32 & (1 << (n - 1))) != 0)
			*v = (int32_t)(u32 | ((uint32_t)-1) << n);
		else
			*v = (int32_t)u32;
	}

	return res;
}


static inline int
h265_bs_write_bits_i(struct h265_bitstream *bs, int32_t v, uint32_t n)
{
	return h265_bs_write_bits_u(bs, ((uint32_t)v) & ((1 << n) - 1), n);
}


/**
 * 9.1 Parsing process for Exp-Golomb codes.
 */
static inline int h265_bs_read_bits_se(struct h265_bitstream *bs, int32_t *v)
{
	int res = 0;
	uint32_t u32 = 0;

	res = h265_bs_read_bits_ue(bs, &u32);
	if (res >= 0) {
		*v = (u32 & 1) ? (((int32_t)u32 + 1) / 2)
			       : (-((int32_t)u32 + 1) / 2);
	}
	return res;
}


/**
 * 9.1 Parsing process for Exp-Golomb codes.
 */
static inline int h265_bs_write_bits_se(struct h265_bitstream *bs, int32_t v)
{
	if (v <= 0)
		return h265_bs_write_bits_ue(bs, (uint32_t)(-2 * v));
	else
		return h265_bs_write_bits_ue(bs, (uint32_t)(2 * v - 1));
}


#endif /* !_H265_BITSTREAM_H_ */
