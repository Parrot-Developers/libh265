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

#ifndef _H265_CTX_H_
#define _H265_CTX_H_


struct h265_ctx;


struct h265_ctx_cbs {
	void (*nalu_begin)(struct h265_ctx *ctx,
			   enum h265_nalu_type type,
			   const uint8_t *buf,
			   size_t len,
			   const struct h265_nalu_header *nh,
			   void *userdata);

	void (*nalu_end)(struct h265_ctx *ctx,
			 enum h265_nalu_type type,
			 const uint8_t *buf,
			 size_t len,
			 const struct h265_nalu_header *nh,
			 void *userdata);

	void (*au_end)(struct h265_ctx *ctx, void *userdata);

	void (*vps)(struct h265_ctx *ctx,
		    const uint8_t *buf,
		    size_t len,
		    const struct h265_vps *vps,
		    void *userdata);

	void (*sps)(struct h265_ctx *ctx,
		    const uint8_t *buf,
		    size_t len,
		    const struct h265_sps *sps,
		    void *userdata);

	void (*pps)(struct h265_ctx *ctx,
		    const uint8_t *buf,
		    size_t len,
		    const struct h265_pps *pps,
		    void *userdata);

	void (*aud)(struct h265_ctx *ctx,
		    const uint8_t *buf,
		    size_t len,
		    const struct h265_aud *aud,
		    void *userdata);

	void (*sei)(struct h265_ctx *ctx,
		    enum h265_sei_type type,
		    const uint8_t *buf,
		    size_t len,
		    void *userdata);

	void (*sei_user_data_unregistered)(
		struct h265_ctx *ctx,
		const uint8_t *buf,
		size_t len,
		const struct h265_sei_user_data_unregistered *sei,
		void *userdata);

	void (*sei_recovery_point)(struct h265_ctx *ctx,
				   const uint8_t *buf,
				   size_t len,
				   const struct h265_sei_recovery_point *sei,
				   void *userdata);

	void (*sei_time_code)(struct h265_ctx *ctx,
			      const uint8_t *buf,
			      size_t len,
			      const struct h265_sei_time_code *sei,
			      void *userdata);

	void (*sei_mastering_display_colour_volume)(
		struct h265_ctx *ctx,
		const uint8_t *buf,
		size_t len,
		const struct h265_sei_mastering_display_colour_volume *sei,
		void *userdata);

	void (*sei_content_light_level)(
		struct h265_ctx *ctx,
		const uint8_t *buf,
		size_t len,
		const struct h265_sei_content_light_level *sei,
		void *userdata);
};


H265_API
int h265_ctx_new(struct h265_ctx **ret_obj);


H265_API
int h265_ctx_destroy(struct h265_ctx *ctx);


H265_API
int h265_ctx_clear_nalu(struct h265_ctx *ctx);


H265_API
int h265_ctx_is_nalu_unknown(struct h265_ctx *ctx);


H265_API int h265_ctx_set_nalu_header(struct h265_ctx *ctx,
				      const struct h265_nalu_header *nh);


H265_API
int h265_ctx_set_aud(struct h265_ctx *ctx, const struct h265_aud *aud);


H265_API
int h265_ctx_set_vps(struct h265_ctx *ctx, const struct h265_vps *vps);


H265_API
int h265_ctx_set_sps(struct h265_ctx *ctx, const struct h265_sps *sps);


H265_API
int h265_ctx_set_pps(struct h265_ctx *ctx, const struct h265_pps *pps);


H265_API
const struct h265_vps *h265_ctx_get_vps(struct h265_ctx *ctx);


H265_API
const struct h265_sps *h265_ctx_get_sps(struct h265_ctx *ctx);


H265_API
const struct h265_pps *h265_ctx_get_pps(struct h265_ctx *ctx);


H265_API
int h265_ctx_add_sei(struct h265_ctx *ctx, const struct h265_sei *sei);


H265_API
int h265_ctx_get_sei_count(struct h265_ctx *ctx);


H265_API uint64_t
h265_ctx_sei_time_code_to_ts(struct h265_ctx *ctx,
			     const struct h265_sei_time_code *sei);


H265_API uint64_t
h265_ctx_sei_time_code_to_us(struct h265_ctx *ctx,
			     const struct h265_sei_time_code *sei);


H265_API
int h265_ctx_get_info(struct h265_ctx *ctx, struct h265_info *info);


#endif /* !_H265_CTX_H_ */
