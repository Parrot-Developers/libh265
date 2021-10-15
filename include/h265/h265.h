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

#ifndef _H265_H_
#define _H265_H_

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* To be used for all public API */
#ifdef H265_API_EXPORTS
#	ifdef _WIN32
#		define H265_API __declspec(dllexport)
#	else /* !_WIN32 */
#		define H265_API __attribute__((visibility("default")))
#	endif /* !_WIN32 */
#else /* !H265_API_EXPORTS */
#	define H265_API
#endif /* !H265_API_EXPORTS */

#include "h265/h265_types.h"

#include "h265/h265_bitstream.h"

#include "h265/h265_ctx.h"

#include "h265/h265_dump.h"
#include "h265/h265_reader.h"
#include "h265/h265_writer.h"


H265_API int h265_get_info(const uint8_t *vps,
			   size_t vps_len,
			   const uint8_t *sps,
			   size_t sps_len,
			   const uint8_t *pps,
			   size_t pps_len,
			   struct h265_info *info);


H265_API int h265_sar_to_aspect_ratio_idc(unsigned int sar_width,
					  unsigned int sar_height);


/* Note: this function expects start code length to be 4 bytes;
 * 3 bytes start codes are not supported */
H265_API int h265_byte_stream_to_hvcc(uint8_t *data, size_t len);


H265_API int h265_hvcc_to_byte_stream(uint8_t *data, size_t len);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_H265_H_ */
