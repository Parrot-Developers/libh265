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

#ifndef _H265_PRIV_H_
#define _H265_PRIV_H_

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#	include <winsock2.h>
#else /* !_WIN32 */
#	include <arpa/inet.h>
#endif /* !_WIN32 */

#define ULOG_TAG h265
#include <ulog.h>

#include <h265/h265.h>


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


struct h265_ctx {
	struct h265_nalu_header nalu_header;

	int first_vcl_of_current_frame_found;

	int nalu_unknown;

	struct h265_aud aud;

	struct h265_vps *vps;
	struct h265_vps *vps_table[16];

	struct h265_sps *sps;
	struct h265_sps *sps_table[16];

	struct h265_pps *pps;
	struct h265_pps *pps_table[64];

	struct h265_sei *sei_table;
	uint32_t sei_count;
};


int h265_get_info_from_ps(const struct h265_vps *vps,
			  const struct h265_sps *sps,
			  const struct h265_pps *pps,
			  struct h265_info *info);


static int h265_ctx_clear_sei_table(struct h265_ctx *ctx);


int h265_ctx_add_sei_internal(struct h265_ctx *ctx, struct h265_sei **ret_obj);


int h265_write_one_sei(struct h265_bitstream *bs,
		       struct h265_ctx *ctx,
		       const struct h265_sei *sei);


int h265_sei_update_internal_buf(struct h265_sei *sei);


#endif /* !_H265_PRIV_H_ */
