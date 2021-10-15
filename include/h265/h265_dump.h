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

#ifndef _H265_DUMP_H_
#define _H265_DUMP_H_


struct json_object;
struct h265_dump;


enum h265_dump_type {
	H265_DUMP_TYPE_JSON,
};


struct h265_dump_cfg {
	enum h265_dump_type type;
};


H265_API
int h265_dump_new(const struct h265_dump_cfg *cfg, struct h265_dump **ret_obj);


H265_API
int h265_dump_destroy(struct h265_dump *dump);


H265_API
int h265_dump_clear(struct h265_dump *dump);


H265_API
int h265_dump_get_json_object(struct h265_dump *dump,
			      struct json_object **jobj);


H265_API
int h265_dump_get_json_str(struct h265_dump *dump, const char **str);


H265_API
int h265_dump_nalu(struct h265_dump *dump,
		   struct h265_ctx *ctx,
		   uint32_t flags);


#endif /* !_H265_DUMP_H_ */
