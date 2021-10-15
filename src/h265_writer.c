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


#define H265_SYNTAX_OP_NAME write
#define H265_SYNTAX_OP_KIND H265_SYNTAX_OP_KIND_WRITE

#define H265_BITS(_f, _n) H265_WRITE_BITS(_f, _n)
#define H265_BITS_U(_f, _n) H265_WRITE_BITS_U(_f, _n)
#define H265_BITS_I(_f, _n) H265_WRITE_BITS_I(_f, _n)
#define H265_BITS_UE(_f) H265_WRITE_BITS_UE(_f)
#define H265_BITS_SE(_f) H265_WRITE_BITS_SE(_f)

#define H265_BITS_RBSP_TRAILING()                                              \
	do {                                                                   \
		int _res = h265_bs_write_rbsp_trailing_bits(bs);               \
		ULOG_ERRNO_RETURN_ERR_IF(_res < 0, -_res);                     \
	} while (0)

#include "h265_syntax.h"


int h265_write_nalu(struct h265_bitstream *bs, struct h265_ctx *ctx)
{
	return _h265_write_nalu(bs, ctx, NULL, NULL);
}


int h265_write_one_sei(struct h265_bitstream *bs,
		       struct h265_ctx *ctx,
		       const struct h265_sei *sei)
{
	return _h265_write_one_sei(bs, ctx, NULL, NULL, sei);
}
