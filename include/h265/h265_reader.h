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

#ifndef _H265_READER_H_
#define _H265_READER_H_


struct h265_reader;


H265_API
int h265_reader_new(const struct h265_ctx_cbs *cbs,
		    void *userdata,
		    struct h265_reader **ret_obj);


H265_API
int h265_reader_destroy(struct h265_reader *reader);


H265_API
int h265_reader_stop(struct h265_reader *reader);


H265_API
struct h265_ctx *h265_reader_get_ctx(struct h265_reader *reader);


H265_API
int h265_reader_parse(struct h265_reader *reader,
		      uint32_t flags,
		      const uint8_t *buf,
		      size_t len,
		      size_t *off);


H265_API
int h265_reader_parse_nalu(struct h265_reader *reader,
			   uint32_t flags,
			   const uint8_t *buf,
			   size_t len);


H265_API
int h265_parse_nalu_header(const uint8_t *buf,
			   size_t len,
			   struct h265_nalu_header *nh);


H265_API
int h265_parse_vps(const uint8_t *buf, size_t len, struct h265_vps *vps);


H265_API
int h265_parse_sps(const uint8_t *buf, size_t len, struct h265_sps *sps);


H265_API
int h265_parse_pps(const uint8_t *buf, size_t len, struct h265_pps *pps);


#endif /* !_H265_READER_H_ */
