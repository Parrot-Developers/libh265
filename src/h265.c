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

ULOG_DECLARE_TAG(h265);


/* E.3.1 "aspect_ratio_idc" */
static const unsigned int h265_sar[17][2] = {
	{1, 1},
	{1, 1},
	{12, 11},
	{10, 11},
	{16, 11},
	{40, 33},
	{24, 11},
	{20, 11},
	{32, 11},
	{80, 33},
	{18, 11},
	{15, 11},
	{64, 33},
	{160, 99},
	{4, 3},
	{3, 2},
	{2, 1},
};


int h265_get_info_from_ps(const struct h265_vps *vps,
			  const struct h265_sps *sps,
			  const struct h265_pps *pps,
			  struct h265_info *info)
{
	ULOG_ERRNO_RETURN_ERR_IF(vps == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(sps == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(pps == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(info == NULL, EINVAL);

	*info = (const struct h265_info){0};

	info->width = sps->pic_width_in_luma_samples;
	info->height = sps->pic_height_in_luma_samples;

	info->bit_depth_luma = sps->bit_depth_luma_minus8 + 8;

	if (sps->conformance_window_flag) {
		/* See Table 6-1 */
		uint32_t sub_width_c = (sps->chroma_format_idc == 1 ||
					sps->chroma_format_idc == 2)
					       ? 2
					       : 1;
		uint32_t sub_height_c = (sps->chroma_format_idc == 1) ? 2 : 1;

		info->crop_left = sps->conf_win_left_offset * sub_width_c;
		info->crop_top = sps->conf_win_top_offset * sub_height_c;
		info->crop_width = info->width -
				   sub_width_c * (sps->conf_win_left_offset +
						  sps->conf_win_right_offset);
		info->crop_height =
			info->height -
			sub_height_c * (sps->conf_win_top_offset +
					sps->conf_win_bottom_offset);
	} else {
		info->crop_left = 0;
		info->crop_top = 0;
		info->crop_width = info->width;
		info->crop_height = info->height;
	}

	if (vps->vps_timing_info_present_flag) {
		info->num_units_in_tick = vps->vps_num_units_in_tick;
		info->time_scale = vps->vps_time_scale;
	}

	info->sar_width = 1;
	info->sar_height = 1;
	if (sps->vui_parameters_present_flag) {
		if (sps->vui.aspect_ratio_info_present_flag) {
			if (sps->vui.aspect_ratio_idc ==
			    H265_ASPECT_RATIO_EXTENDED_SAR) {
				info->sar_width = sps->vui.sar_width;
				info->sar_height = sps->vui.sar_height;
			} else if (sps->vui.aspect_ratio_idc <= 16) {
				info->sar_width =
					h265_sar[sps->vui.aspect_ratio_idc][0];
				info->sar_height =
					h265_sar[sps->vui.aspect_ratio_idc][1];
			}
		}

		info->full_range = sps->vui.video_full_range_flag;

		if (sps->vui.colour_description_present_flag) {
			info->colour_description_present = 1;
			info->colour_primaries = sps->vui.colour_primaries;
			info->transfer_characteristics =
				sps->vui.transfer_characteristics;
			info->matrix_coefficients = sps->vui.matrix_coeffs;
		} else {
			/* No colour description present, set to default
			 * values (2 means unspecified) */
			info->colour_primaries = 2;
			info->transfer_characteristics = 2;
			info->matrix_coefficients = 2;
		}

		if (sps->vui.vui_timing_info_present_flag) {
			if (info->num_units_in_tick != 0 &&
			    info->num_units_in_tick !=
				    sps->vui.vui_num_units_in_tick) {
				ULOGW("num_units_in_tick mismatch: "
				      "VPS:%u VUI:%u",
				      info->num_units_in_tick,
				      sps->vui.vui_num_units_in_tick);
			} else {
				info->num_units_in_tick =
					sps->vui.vui_num_units_in_tick;
			}
			if (info->time_scale != 0 &&
			    info->time_scale != sps->vui.vui_time_scale) {
				ULOGW("time_scale mismatch: VPS:%u VUI:%u",
				      info->time_scale,
				      sps->vui.vui_time_scale);
			} else {
				info->time_scale = sps->vui.vui_time_scale;
			}
		}

		if (sps->vui.vui_hrd_parameters_present_flag &&
		    sps->vui.hrd.nal_hrd_parameters_present_flag) {
			info->nal_hrd_bitrate =
				(sps->vui.hrd.sub_layers[0]
					 .nal_hrd.cpbs[0]
					 .bit_rate_du_value_minus1 +
				 1) * 1
				<< (6 + sps->vui.hrd.bit_rate_scale);

			info->nal_hrd_cpb_size =
				(sps->vui.hrd.sub_layers[0]
					 .nal_hrd.cpbs[0]
					 .size_value_minus1 +
				 1) * 1
				<< (4 + sps->vui.hrd.cpb_size_scale);
		}

		if (sps->vui.vui_hrd_parameters_present_flag &&
		    sps->vui.hrd.vcl_hrd_parameters_present_flag) {
			info->vcl_hrd_bitrate =
				(sps->vui.hrd.sub_layers[0]
					 .vcl_hrd.cpbs[0]
					 .bit_rate_du_value_minus1 +
				 1) * 1
				<< (6 + sps->vui.hrd.bit_rate_scale);

			info->vcl_hrd_cpb_size =
				(sps->vui.hrd.sub_layers[0]
					 .vcl_hrd.cpbs[0]
					 .size_value_minus1 +
				 1) * 1
				<< (4 + sps->vui.hrd.cpb_size_scale);
		}
	}

	info->framerate =
		(info->num_units_in_tick != 0)
			? ((float)info->time_scale) / info->num_units_in_tick
			: 0.0f;
	info->framerate_num = info->time_scale;
	info->framerate_den = info->num_units_in_tick;

	return 0;
}

int h265_get_info(const uint8_t *vps,
		  size_t vps_len,
		  const uint8_t *sps,
		  size_t sps_len,
		  const uint8_t *pps,
		  size_t pps_len,
		  struct h265_info *info)
{
	int res;
	struct h265_vps *parsed_vps = NULL;
	struct h265_sps *parsed_sps = NULL;
	struct h265_pps *parsed_pps = NULL;

	parsed_vps = calloc(1, sizeof(*parsed_vps));
	if (parsed_vps == NULL) {
		res = -ENOMEM;
		goto out;
	}

	res = h265_parse_vps(vps, vps_len, parsed_vps);
	if (res < 0)
		goto out;

	parsed_sps = calloc(1, sizeof(*parsed_sps));
	if (parsed_sps == NULL) {
		res = -ENOMEM;
		goto out;
	}

	res = h265_parse_sps(sps, sps_len, parsed_sps);
	if (res < 0)
		goto out;

	parsed_pps = calloc(1, sizeof(*parsed_pps));
	if (parsed_pps == NULL) {
		res = -ENOMEM;
		goto out;
	}

	res = h265_parse_pps(pps, pps_len, parsed_pps);
	if (res < 0)
		goto out;

	res = h265_get_info_from_ps(parsed_vps, parsed_sps, parsed_pps, info);

out:
	h265_pps_clear(parsed_pps);
	free(parsed_pps);
	free(parsed_sps);
	free(parsed_vps);
	return res;
}


int h265_sar_to_aspect_ratio_idc(unsigned int sar_width,
				 unsigned int sar_height)
{
	unsigned int idx, found = 0;

	/* Start at index 1 as 0 is 'unspecified' */
	for (idx = 1; idx < ARRAY_SIZE(h265_sar); idx++) {
		if ((h265_sar[idx][0] == sar_width) &&
		    (h265_sar[idx][1] == sar_height)) {
			found = 1;
			break;
		}
	}

	return found ? idx : H265_ASPECT_RATIO_EXTENDED_SAR;
}


static int find_start_code(const uint8_t *buf, size_t len, size_t *start)
{
	const uint8_t *p = buf;

	while (len >= 4) {
		/* Search for the next 0x00 byte */
		if (*p != 0x00)
			goto next;

		/* Is it a 00 00 00 01 sequence? */
		if (p[1] == 0x00 && p[2] == 0x00 && p[3] == 0x01) {
			*start = p - buf;
			return 0;
		}

		/* clang-format off */
next:
		/* clang-format on */
		p++;
		len--;
	}

	return -ENOENT;
}


int h265_byte_stream_to_hvcc(uint8_t *data, size_t len)
{
	int res;
	uint32_t nalu_len_be;
	size_t start, next_start, nalu_len;

	ULOG_ERRNO_RETURN_ERR_IF(data == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(len == 0, EINVAL);

	/* First NALU start code */
	res = find_start_code(data, len, &start);
	if (res < 0) {
		if (res != -ENOENT)
			return res;
		ULOGW("%s: no start code found", __func__);
		return 0;
	}
	data += start;
	len -= start;

	while (len > 4) {
		/* Find the next NALU start code */
		res = find_start_code(data + 4, len - 4, &next_start);
		if (res == -ENOENT) {
			/* Last NALU */
			nalu_len = len - 4;
		} else if (res < 0) {
			return res;
		} else {
			nalu_len = next_start;
		}
		nalu_len_be = htonl(nalu_len);
		memcpy(data, &nalu_len_be, sizeof(uint32_t));
		data += 4 + nalu_len;
		len -= (4 + nalu_len);
	}

	return 0;
}


int h265_hvcc_to_byte_stream(uint8_t *data, size_t len)
{
	uint32_t nalu_len, start_code = htonl(0x00000001);
	size_t offset = 0;

	ULOG_ERRNO_RETURN_ERR_IF(data == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(len == 0, EINVAL);

	while (offset < len) {
		memcpy(&nalu_len, data, sizeof(uint32_t));
		nalu_len = ntohl(nalu_len);
		if (nalu_len == 0) {
			ULOGE("%s: invalid NALU size (%u)", __func__, nalu_len);
			return -EPROTO;
		}
		memcpy(data, &start_code, sizeof(uint32_t));
		data += 4 + nalu_len;
		offset += 4 + nalu_len;
	}

	return 0;
}
