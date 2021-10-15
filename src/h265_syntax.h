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

#ifndef _H265_SYNTAX_H_
#define _H265_SYNTAX_H_

#include <inttypes.h>
#include <math.h>

#include "h265_syntax_ops.h"


static int H265_SYNTAX_FCT(sei_data)(struct h265_bitstream *bs,
				     /* clang-format off */
				     const uint8_t *H265_SYNTAX_CONST*buf,
				     /* clang-format on */
				     H265_SYNTAX_CONST size_t *len)
{
#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
	ULOG_ERRNO_RETURN_ERR_IF(!h265_bs_byte_aligned(bs), EIO);
	*buf = bs->cdata + bs->off;
	*len = bs->len - bs->off;
#else
	ULOG_ERRNO_RETURN_ERR_IF(*len != 0 && *buf == NULL, EIO);
	H265_BEGIN_ARRAY(data);
	for (uint32_t i = 0; i < *len; i++)
		H265_BITS((*buf)[i], 8);
	H265_END_ARRAY(data);
#endif

	return 0;
}


/**
 * D.2.7 User data unregistered SEI message syntax
 */
static int H265_SYNTAX_FCT(sei_user_data_unregistered)(
	struct h265_bitstream *bs,
	struct h265_ctx *ctx,
	H265_SYNTAX_CONST struct h265_sei_user_data_unregistered *sei)
{
	int res = 0;

	H265_BEGIN_ARRAY(uuid);
	for (uint32_t i = 0; i < 16; i++)
		H265_BITS(sei->uuid[i], 8);
	H265_END_ARRAY(uuid);

	res = H265_SYNTAX_FCT(sei_data)(bs, &sei->buf, &sei->len);
	ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

	return 0;
}


/**
 * D.2.8 Recovery point SEI message syntax
 */
static int H265_SYNTAX_FCT(sei_recovery_point)(
	struct h265_bitstream *bs,
	struct h265_ctx *ctx,
	H265_SYNTAX_CONST struct h265_sei_recovery_point *sei)
{
	H265_BITS_SE(sei->recovery_poc_cnt);
	H265_BITS(sei->exact_match_flag, 1);
	H265_BITS(sei->broken_link_flag, 1);
	return 0;
}


/**
 * D.2.27 Time code SEI message syntax
 */
static int H265_SYNTAX_FCT(sei_time_code)(
	struct h265_bitstream *bs,
	struct h265_ctx *ctx,
	H265_SYNTAX_CONST struct h265_sei_time_code *sei)
{
	H265_BITS(sei->num_clock_ts, 2);

	H265_BEGIN_ARRAY(clock_ts);
	for (uint32_t i = 0; i < sei->num_clock_ts; i++) {
		H265_BEGIN_ARRAY_ITEM();
		H265_BITS(sei->clock_ts[i].clock_timestamp_flag, 1);
		if (!sei->clock_ts[i].clock_timestamp_flag)
			goto next_ts;

		H265_BITS(sei->clock_ts[i].units_field_based_flag, 1);
		H265_BITS(sei->clock_ts[i].counting_type, 5);
		H265_BITS(sei->clock_ts[i].full_timestamp_flag, 1);
		H265_BITS(sei->clock_ts[i].discontinuity_flag, 1);
		H265_BITS(sei->clock_ts[i].cnt_dropped_flag, 1);
		H265_BITS(sei->clock_ts[i].n_frames, 9);

		if (sei->clock_ts[i].full_timestamp_flag) {
			H265_BITS(sei->clock_ts[i].seconds_value, 6);
			H265_BITS(sei->clock_ts[i].minutes_value, 6);
			H265_BITS(sei->clock_ts[i].hours_value, 5);
		} else {
			H265_BITS(sei->clock_ts[i].seconds_flag, 1);
			if (!sei->clock_ts[i].seconds_flag)
				goto time_offset;
			H265_BITS(sei->clock_ts[i].seconds_value, 6);
			H265_BITS(sei->clock_ts[i].minutes_flag, 1);
			if (!sei->clock_ts[i].minutes_flag)
				goto time_offset;
			H265_BITS(sei->clock_ts[i].minutes_value, 6);
			H265_BITS(sei->clock_ts[i].hours_flag, 1);
			if (!sei->clock_ts[i].hours_flag)
				goto time_offset;
			H265_BITS(sei->clock_ts[i].hours_value, 5);
		}

		/* clang-format off */
time_offset:
		/* clang-format on */
		H265_BITS(sei->clock_ts[i].time_offset_length, 5);
		if (sei->clock_ts[i].time_offset_length > 0)
			H265_BITS_I(sei->clock_ts[i].time_offset_value,
				    sei->clock_ts[i].time_offset_length);

		/* clang-format off */
next_ts:
		/* clang-format on */
		H265_END_ARRAY_ITEM();
	}
	H265_END_ARRAY(clock_ts);
	return 0;
}


/**
 * D.2.28 Mastering display colour volume SEI message syntax
 */
static int H265_SYNTAX_FCT(sei_mastering_display_colour_volume)(
	struct h265_bitstream *bs,
	struct h265_ctx *ctx,
	H265_SYNTAX_CONST struct h265_sei_mastering_display_colour_volume *sei)
{
	for (uint32_t i = 0; i < 3; i++) {
		H265_BITS(sei->display_primaries_x[i], 16);
		H265_BITS(sei->display_primaries_y[i], 16);
	}
	H265_BITS(sei->white_point_x, 16);
	H265_BITS(sei->white_point_y, 16);
	H265_BITS(sei->max_display_mastering_luminance, 32);
	H265_BITS(sei->min_display_mastering_luminance, 32);
	return 0;
}


/**
 * D.2.35 Content light level information SEI message syntax
 */
static int H265_SYNTAX_FCT(sei_content_light_level)(
	struct h265_bitstream *bs,
	struct h265_ctx *ctx,
	H265_SYNTAX_CONST struct h265_sei_content_light_level *sei)
{
	H265_BITS(sei->max_content_light_level, 16);
	H265_BITS(sei->max_pic_average_light_level, 16);
	return 0;
}


/**
 * 7.3.1.2 NAL unit header syntax
 */
static int H265_SYNTAX_FCT(nalu_header)(
	struct h265_bitstream *bs,
	H265_SYNTAX_CONST struct h265_nalu_header *nh)
{
	H265_BITS(nh->forbidden_zero_bit, 1);
	ULOG_ERRNO_RETURN_ERR_IF(nh->forbidden_zero_bit != 0, EIO);
	H265_BITS(nh->nal_unit_type, 6);
	H265_BITS(nh->nuh_layer_id, 6);
	H265_BITS(nh->nuh_temporal_id_plus1, 3);

	return 0;
}


static int H265_SYNTAX_FCT(one_sei)(struct h265_bitstream *bs,
				    struct h265_ctx *ctx,
				    const struct h265_ctx_cbs *cbs,
				    void *userdata,
				    H265_SYNTAX_CONST struct h265_sei *sei)
{
#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
	int bit = 0;
#endif

	switch (sei->type) {
	case H265_SEI_TYPE_USER_DATA_UNREGISTERED:
		H265_SEI(user_data_unregistered);
		break;

	case H265_SEI_TYPE_RECOVERY_POINT:
		H265_SEI(recovery_point);
		break;

	case H265_SEI_TYPE_TIME_CODE:
		H265_SEI(time_code);
		break;

	case H265_SEI_TYPE_MASTERING_DISPLAY_COLOUR_VOLUME:
		H265_SEI(mastering_display_colour_volume);
		break;

	case H265_SEI_TYPE_CONTENT_LIGHT_LEVEL:
		H265_SEI(content_light_level);
		break;

	default:
		/* TODO */
		return 0;
	}

	/* Align bitstream if needed */
#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
	/* Should be 1 followed by 0, but ignore erroneous bit streams */
	while (!h265_bs_byte_aligned(bs))
		H265_BITS(bit, 1);
#else
	if (!h265_bs_byte_aligned(bs))
		H265_BITS_RBSP_TRAILING();
#endif

	return 0;
}


/**
 * 7.3.2.4 Supplemental enhancement information RBSP syntax
 */
static int H265_SYNTAX_FCT(sei)(struct h265_bitstream *bs,
				struct h265_ctx *ctx,
				const struct h265_ctx_cbs *cbs,
				void *userdata)
{
	int res = 0;
	uint32_t payload_type = 0;
	uint32_t payload_size = 0;
	struct h265_sei *sei = NULL;

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ

	struct h265_bitstream bs2;
	do {
		H265_BEGIN_ARRAY_ITEM();

		res = h265_bs_read_bits_ff_coded(bs, &payload_type);
		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

		res = h265_bs_read_bits_ff_coded(bs, &payload_size);
		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

		/* Allocate new SEI in internal table */
		res = h265_ctx_add_sei_internal(ctx, &sei);
		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
		sei->type = payload_type;

		/* Setup raw buffer */
		sei->raw.buf = malloc(payload_size);
		ULOG_ERRNO_RETURN_ERR_IF(sei->raw.buf == NULL, ENOMEM);
		sei->raw.len = payload_size;

		for (uint32_t i = 0; i < sei->raw.len; i++)
			H265_BITS(sei->raw.buf[i], 8);

		/* Notify callback */
		H265_CB(ctx,
			cbs,
			userdata,
			sei,
			sei->type,
			sei->raw.buf,
			sei->raw.len);

		/* Construct a bitstream to parse SEI
		 * (without emulation prevention) */
		h265_bs_cinit(&bs2, sei->raw.buf, sei->raw.len, 0);
		res = H265_SYNTAX_FCT(one_sei)(&bs2, ctx, cbs, userdata, sei);
		h265_bs_clear(&bs2);
		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

		H265_END_ARRAY_ITEM();
	} while (h265_bs_more_rbsp_data(bs));

#elif H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_WRITE

	ULOG_ERRNO_RETURN_ERR_IF(ctx->sei_count == 0, EIO);
	for (uint32_t i = 0; i < ctx->sei_count; i++) {
		H265_BEGIN_ARRAY_ITEM();
		sei = &ctx->sei_table[i];

		ULOG_ERRNO_RETURN_ERR_IF(sei->raw.buf == NULL, EIO);
		ULOG_ERRNO_RETURN_ERR_IF(sei->raw.len == 0, EIO);

		payload_type = sei->type;
		res = h265_bs_write_bits_ff_coded(bs, payload_type);
		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

		payload_size = sei->raw.len;
		res = h265_bs_write_bits_ff_coded(bs, payload_size);
		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

		/* Directly write raw buffer, SEI should already be encoded */
		for (uint32_t i = 0; i < sei->raw.len; i++)
			H265_BITS(sei->raw.buf[i], 8);

		H265_END_ARRAY_ITEM();
	}

#elif H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_DUMP

	ULOG_ERRNO_RETURN_ERR_IF(ctx->sei_count == 0, EIO);
	for (uint32_t i = 0; i < ctx->sei_count; i++) {
		H265_BEGIN_ARRAY_ITEM();
		sei = &ctx->sei_table[i];

		/* Use fake number of bits for dump (no actual bitstream) */
		payload_type = sei->type;
		H265_BITS(payload_type, 0);
		payload_size = sei->raw.len;
		H265_BITS(payload_size, 0);

		res = H265_SYNTAX_FCT(one_sei)(bs, ctx, cbs, userdata, sei);
		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

		H265_END_ARRAY_ITEM();
	}

#else
#	error "Unsupported H265_SYNTAX_OP_KIND"
#endif

	H265_BITS_RBSP_TRAILING();

	return 0;
}


/**
 * 7.3.2.5 Access unit delimiter RBSP syntax
 */
static int H265_SYNTAX_FCT(aud)(struct h265_bitstream *bs,
				H265_SYNTAX_CONST struct h265_aud *aud)
{
	H265_BITS(aud->pic_type, 3);
	H265_BITS_RBSP_TRAILING();
	return 0;
}


/**
 * 7.3.3 Profile, tier and level syntax
 *
 * This function is not present in the spec. It is used as a subroutine in
 * profile_tier_level.
 */
static int H265_SYNTAX_FCT(ptl_core)(struct h265_bitstream *bs,
				     int profile_present_flag,
				     int level_present_flag,
				     struct h265_ptl_core *ptl)
{
	if (profile_present_flag) {
		H265_BITS(ptl->profile_space, 2);
		H265_BITS(ptl->tier_flag, 1);
		H265_BITS(ptl->profile_idc, 5);

		H265_BEGIN_ARRAY(profile_compatibility_flag);
		for (int i = 0; i < 32; ++i) {
			H265_BEGIN_ARRAY_ITEM();
			H265_BITS(ptl->profile_compatibility_flag[i], 1);
			H265_END_ARRAY_ITEM();
		}
		H265_END_ARRAY(profile_compatibility_flag);

		H265_BITS(ptl->progressive_source_flag, 1);
		H265_BITS(ptl->interlaced_source_flag, 1);
		H265_BITS(ptl->non_packed_constraint_flag, 1);
		H265_BITS(ptl->frame_only_constraint_flag, 1);

		if ((4 <= ptl->profile_idc && ptl->profile_idc <= 10) ||
		    ptl->profile_compatibility_flag[4] ||
		    ptl->profile_compatibility_flag[5] ||
		    ptl->profile_compatibility_flag[6] ||
		    ptl->profile_compatibility_flag[7] ||
		    ptl->profile_compatibility_flag[8] ||
		    ptl->profile_compatibility_flag[9] ||
		    ptl->profile_compatibility_flag[10]) {
			H265_BITS(ptl->max_12bit_constraint_flag, 1);
			H265_BITS(ptl->max_10bit_constraint_flag, 1);
			H265_BITS(ptl->max_8bit_constraint_flag, 1);
			H265_BITS(ptl->max_422chroma_constraint_flag, 1);
			H265_BITS(ptl->max_420chroma_constraint_flag, 1);
			H265_BITS(ptl->max_monochrome_constraint_flag, 1);
			H265_BITS(ptl->intra_constraint_flag, 1);
			H265_BITS(ptl->one_picture_only_constraint_flag, 1);
			H265_BITS(ptl->lower_bit_rate_constraint_flag, 1);

			if (ptl->profile_idc == 5 ||
			    ptl->profile_compatibility_flag[5] ||
			    ptl->profile_idc == 9 ||
			    ptl->profile_compatibility_flag[9] ||
			    ptl->profile_idc == 10 ||
			    ptl->profile_compatibility_flag[10]) {
				H265_BITS(ptl->max_14bit_constraint_flag, 1);

				/* Ignore reserved_zero_33bits */
				H265_BITS((uint32_t){0}, 33);
			} else {
				/* Ignore reserved_zero_34bits */
				H265_BITS((uint32_t){0}, 34);
			}
		} else if (ptl->profile_idc == 2 ||
			   ptl->profile_compatibility_flag[2]) {
			/* Ignore reserved_zero_7bits */
			H265_BITS((uint32_t){0}, 7);

			H265_BITS(ptl->one_picture_only_constraint_flag, 1);

			/* Ignore reserved_zero_35bits */
			H265_BITS((uint32_t){0}, 35);
		} else {
			/* Ignore reserved_zero_43bits */
			H265_BITS((uint32_t){0}, 43);
		}

		if ((1 <= ptl->profile_idc && ptl->profile_idc <= 5) ||
		    ptl->profile_idc == 9 ||
		    ptl->profile_compatibility_flag[1] ||
		    ptl->profile_compatibility_flag[2] ||
		    ptl->profile_compatibility_flag[3] ||
		    ptl->profile_compatibility_flag[4] ||
		    ptl->profile_compatibility_flag[5] ||
		    ptl->profile_compatibility_flag[9]) {
			H265_BITS(ptl->inbld_flag, 1);
		} else {
			/* Ignore reserved_zero_bit */
			H265_BITS((uint32_t){0}, 1);
		}
	}

	if (level_present_flag)
		H265_BITS(ptl->level_idc, 8);

	return 0;
}


/**
 * 7.3.3 Profile, tier and level syntax
 */
static int
	H265_SYNTAX_FCT(profile_tier_level)(struct h265_bitstream *bs,
					    int profile_present_flag,
					    uint32_t max_sub_layers_minus1,
					    struct h265_profile_tier_level *ptl)
{
	int res = 0;

	res = H265_SYNTAX_FCT(ptl_core)(
		bs, profile_present_flag, 1, &ptl->general);

	ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

	H265_BEGIN_ARRAY(sub_layer_present_flags);
	for (uint32_t i = 0; i < max_sub_layers_minus1; ++i) {
		H265_BEGIN_ARRAY_ITEM();
		H265_BITS(ptl->sub_layer_present_flags[i].profile, 1);
		H265_BITS(ptl->sub_layer_present_flags[i].level, 1);
		H265_END_ARRAY_ITEM();
	}
	H265_END_ARRAY(sub_layer_present_flags);

	if (max_sub_layers_minus1 > 0) {
		for (uint32_t i = max_sub_layers_minus1; i < 8; ++i) {
			/* Ignore reserved_zero_2bits */
			H265_BITS((uint32_t){0}, 2);
		}
	}

	H265_BEGIN_ARRAY(sub_layers);
	for (uint32_t i = 0; i < max_sub_layers_minus1; ++i) {
		H265_BEGIN_ARRAY_ITEM();

		res = H265_SYNTAX_FCT(ptl_core)(
			bs,
			ptl->sub_layer_present_flags[i].profile,
			ptl->sub_layer_present_flags[i].level,
			&ptl->sub_layers[i]);

		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

		H265_END_ARRAY_ITEM();
	}
	H265_END_ARRAY(sub_layers);

	return 0;
}


/**
 * E.2.3 Sub-layer HRD parameters syntax
 */
static int H265_SYNTAX_FCT(sub_layer_hrd)(struct h265_bitstream *bs,
					  uint32_t cpb_cnt,
					  int sub_pic_hrd_params_present_flag,
					  struct h265_sub_layer_hrd *hrd)
{
	H265_BEGIN_ARRAY(cpbs);
	for (uint32_t i = 0; i < cpb_cnt; ++i) {
		H265_BEGIN_ARRAY_ITEM();

		H265_BITS_UE(hrd->cpbs[i].bit_rate_value_minus1);
		H265_BITS_UE(hrd->cpbs[i].size_value_minus1);
		if (sub_pic_hrd_params_present_flag) {
			H265_BITS_UE(hrd->cpbs[i].size_du_value_minus1);
			H265_BITS_UE(hrd->cpbs[i].bit_rate_du_value_minus1);
		}
		H265_BITS(hrd->cpbs[i].flag, 1);

		H265_END_ARRAY_ITEM();
	}
	H265_END_ARRAY(cpbs);

	return 0;
}


static int H265_SYNTAX_FCT(hrd_common)(struct h265_bitstream *bs,
				       struct h265_hrd *hrd)
{
	uint32_t *dcrdil_minus1 =
		&hrd->du_cpb_removal_delay_increment_length_minus1;

	H265_BITS(hrd->nal_hrd_parameters_present_flag, 1);
	H265_BITS(hrd->vcl_hrd_parameters_present_flag, 1);

	if (hrd->nal_hrd_parameters_present_flag ||
	    hrd->vcl_hrd_parameters_present_flag) {
		H265_BITS(hrd->sub_pic_hrd_params_present_flag, 1);

		if (hrd->sub_pic_hrd_params_present_flag) {
			H265_BITS(hrd->tick_divisor_minus2, 8);
			H265_BITS(*dcrdil_minus1, 5);
			H265_BITS(
				hrd->sub_pic_cpb_params_in_pic_timing_sei_flag,
				1);
			H265_BITS(hrd->dpb_output_delay_du_length_minus1, 5);
		}

		H265_BITS(hrd->bit_rate_scale, 4);
		H265_BITS(hrd->cpb_size_scale, 4);

		if (hrd->sub_pic_hrd_params_present_flag)
			H265_BITS(hrd->cpb_size_du_scale, 4);

		H265_BITS(hrd->initial_cpb_removal_delay_length_minus1, 5);
		H265_BITS(hrd->au_cpb_removal_delay_length_minus1, 5);
		H265_BITS(hrd->dpb_output_delay_length_minus1, 5);
	}

	return 0;
}


/**
 * E.2.2 HRD parameters syntax
 */
static int H265_SYNTAX_FCT(hrd)(struct h265_bitstream *bs,
				int common_inf_present_flag,
				uint32_t max_num_sub_layers_minus1,
				struct h265_hrd *hrd)
{
	int res = 0;

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
	/* These fields have non-zero default values when missing */
	hrd->initial_cpb_removal_delay_length_minus1 = 23;
	hrd->au_cpb_removal_delay_length_minus1 = 23;
	hrd->dpb_output_delay_length_minus1 = 23;
#endif

	if (common_inf_present_flag) {
		res = H265_SYNTAX_FCT(hrd_common)(bs, hrd);
		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
	}

	H265_BEGIN_ARRAY(sub_layers);
	for (uint32_t i = 0; i <= max_num_sub_layers_minus1; ++i) {
		H265_BEGIN_ARRAY_ITEM();

		H265_BITS(hrd->sub_layers[i].fixed_pic_rate_general_flag, 1);
		if (!hrd->sub_layers[i].fixed_pic_rate_general_flag)
			H265_BITS(hrd->sub_layers[i]
					  .fixed_pic_rate_within_cvs_flag,
				  1);
		else
			/* Non-zero default value */
			hrd->sub_layers[i].fixed_pic_rate_within_cvs_flag = 1;

		if (hrd->sub_layers[i].fixed_pic_rate_within_cvs_flag)
			H265_BITS_UE(hrd->sub_layers[i]
					     .elemental_duration_in_tc_minus1);
		else
			H265_BITS(hrd->sub_layers[i].low_delay_hrd_flag, 1);

		if (!hrd->sub_layers[i].low_delay_hrd_flag)
			H265_BITS_UE(hrd->sub_layers[i].cpb_cnt_minus1);

		if (hrd->nal_hrd_parameters_present_flag) {
			res = H265_SYNTAX_FCT(sub_layer_hrd)(
				bs,
				hrd->sub_layers[i].cpb_cnt_minus1 + 1,
				hrd->sub_pic_hrd_params_present_flag,
				&hrd->sub_layers[i].nal_hrd);

			ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
		}

		if (hrd->vcl_hrd_parameters_present_flag) {
			res = H265_SYNTAX_FCT(sub_layer_hrd)(
				bs,
				hrd->sub_layers[i].cpb_cnt_minus1 + 1,
				hrd->sub_pic_hrd_params_present_flag,
				&hrd->sub_layers[i].vcl_hrd);

			ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
		}

		H265_END_ARRAY_ITEM();
	}
	H265_END_ARRAY(sub_layers);

	return 0;
}


static int H265_SYNTAX_FCT(layer_set)(struct h265_bitstream *bs,
				      int *layer_set,
				      uint32_t vps_max_layer_id)
{
	H265_BEGIN_ARRAY(layer_set);
	for (uint32_t j = 0; j <= vps_max_layer_id; ++j) {
		H265_BEGIN_ARRAY_ITEM();
		H265_BITS(layer_set[j], 1);
		H265_BEGIN_ARRAY_ITEM();
	}
	H265_END_ARRAY(layer_set);

	return 0;
}


static int H265_SYNTAX_FCT(vps)(struct h265_bitstream *bs, struct h265_vps *vps)
{
	int res = 0;


	H265_BITS(vps->vps_video_parameter_set_id, 4);
	H265_BITS(vps->vps_base_layer_internal_flag, 1);
	H265_BITS(vps->vps_base_layer_available_flag, 1);
	H265_BITS(vps->vps_max_layers_minus1, 6);
	H265_BITS(vps->vps_max_sub_layers_minus1, 3);
	H265_BITS(vps->vps_temporal_id_nesting_flag, 1);
	H265_BITS(vps->vps_reserved_0xffff_16bits, 16);

	H265_BEGIN_STRUCT(profile_tier_level);
	res = H265_SYNTAX_FCT(profile_tier_level)(
		bs,
		1,
		vps->vps_max_sub_layers_minus1,
		&vps->profile_tier_level);
	H265_END_STRUCT(profile_tier_level);

	ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

	H265_BITS(vps->vps_sub_layer_ordering_info_present_flag, 1);
	uint32_t i = vps->vps_sub_layer_ordering_info_present_flag
			     ? 0
			     : vps->vps_max_sub_layers_minus1;

	H265_BEGIN_ARRAY(vps_sub_layer_ordering_infos);
	for (; i <= vps->vps_max_sub_layers_minus1; ++i) {
		H265_BEGIN_ARRAY_ITEM();

		H265_BITS_UE(vps->vps_max_dec_pic_buffering_minus1[i]);
		H265_BITS_UE(vps->vps_max_num_reorder_pics[i]);
		H265_BITS_UE(vps->vps_max_latency_increase_plus1[i]);

		H265_END_ARRAY_ITEM();
	}
	H265_END_ARRAY(vps_sub_layer_ordering_infos);

	H265_BITS(vps->vps_max_layer_id, 6);
	H265_BITS_UE(vps->vps_num_layer_sets_minus1);

	H265_BEGIN_ARRAY(layer_included_flag);
	for (uint32_t i = 1; i <= vps->vps_num_layer_sets_minus1; ++i) {
		H265_BEGIN_ARRAY_ITEM();

		int *row = vps->layer_id_included_flag[i];
		res = H265_SYNTAX_FCT(layer_set)(
			bs, row, vps->vps_max_layer_id);

		ULOG_ERRNO_RETURN_ERR_IF(res > 0, -res);

		H265_END_ARRAY_ITEM();
	}
	H265_END_ARRAY(layer_included_flag);

	H265_BITS(vps->vps_timing_info_present_flag, 1);
	if (vps->vps_timing_info_present_flag) {
		H265_BITS(vps->vps_num_units_in_tick, 32);
		H265_BITS(vps->vps_time_scale, 32);

		H265_BITS(vps->vps_poc_proportional_to_timing_flag, 1);
		if (vps->vps_poc_proportional_to_timing_flag)
			H265_BITS_UE(vps->vps_num_ticks_poc_diff_one_minus1);

		H265_BITS_UE(vps->vps_num_hrd_parameters);

		H265_BEGIN_ARRAY(hrd_layer_set_idx);
		for (uint32_t i = 0; i < vps->vps_num_hrd_parameters; ++i) {
			H265_BEGIN_ARRAY_ITEM();

			H265_BITS_UE(vps->hrd_layer_set_idx[i]);

			if (i > 0)
				H265_BITS(vps->cprms_present_flag[i], 1);

			H265_BEGIN_STRUCT(hrd);
			res = H265_SYNTAX_FCT(hrd)(
				bs,
				vps->cprms_present_flag[i],
				vps->vps_max_sub_layers_minus1,
				&vps->hrd_parameters[i]);
			H265_END_STRUCT(hrd);

			ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

			H265_END_ARRAY_ITEM();
		}
		H265_END_ARRAY(hrd_layer_set_idx);
	}

	H265_BITS(vps->vps_extension_flag, 1);

	/* TODO: vps_extension_data_flag */

	H265_BITS_RBSP_TRAILING();

	return 0;
}


static int
	H265_SYNTAX_FCT(scaling_list_inner)(struct h265_bitstream *bs,
					    int size_id,
					    int matrix_id,
					    struct h265_scaling_list_data *sl)
{
	H265_BITS(sl->pred_mode_flag[size_id][matrix_id], 1);
	if (!sl->pred_mode_flag[size_id][matrix_id]) {
		H265_BITS_UE(sl->pred_matrix_id_delta[size_id - 2][matrix_id]);
	} else {
		if (size_id > 1)
			H265_BITS_SE(sl->dc_coef_minus8[size_id][matrix_id]);

		int x = 1 << (4 + (size_id << 1));
		int coef_num = x <= 64 ? x : 64;

		for (int i = 0; i < coef_num; ++i)
			H265_BITS_SE(sl->delta_coef[size_id][matrix_id][i]);
	}

	return 0;
}


static int H265_SYNTAX_FCT(scaling_list_data)(struct h265_bitstream *bs,
					      struct h265_scaling_list_data *sl)
{
	int res = 0;

	for (int size_id = 0; size_id < 4; ++size_id) {
		for (int matrix_id = 0; matrix_id < 6; ++matrix_id) {
			res = H265_SYNTAX_FCT(scaling_list_inner)(
				bs, size_id, matrix_id, sl);

			ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
		}
	}

	return 0;
}


static int H265_SYNTAX_FCT(st_ref_pic_set)(
	struct h265_bitstream *bs,
	uint32_t st_rps_idx,
	uint32_t num_short_ref_pic_sets,
	struct h265_st_ref_pic_set *st_rps_table)
{
	struct h265_st_ref_pic_set *st_rps = &st_rps_table[st_rps_idx];

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
	/* Non-zero default values */
	for (size_t i = 0; i < ARRAY_SIZE(st_rps->use_delta_flag); ++i)
		st_rps->use_delta_flag[i] = 1;
#endif

	if (st_rps_idx != 0)
		H265_BITS(st_rps->inter_ref_pic_set_prediction_flag, 1);
	if (st_rps->inter_ref_pic_set_prediction_flag) {
		if (st_rps_idx == num_short_ref_pic_sets)
			H265_BITS_UE(st_rps->delta_idx_minus1);

		H265_BITS(st_rps->delta_rps_sign, 1);
		H265_BITS_UE(st_rps->abs_delta_rps_minus1);

		uint32_t ref_rps_idx =
			st_rps_idx - (st_rps->delta_idx_minus1 + 1);
		struct h265_st_ref_pic_set *ref_rps =
			&st_rps_table[ref_rps_idx];

		int32_t delta_rps = (1 - 2 * st_rps->delta_rps_sign) *
				    (st_rps->abs_delta_rps_minus1 + 1);

		uint32_t num_delta_pocs =
			ref_rps->num_negative_pics + ref_rps->num_positive_pics;

		H265_BEGIN_ARRAY(delta_pocs);
		for (uint32_t j = 0; j <= num_delta_pocs; ++j) {
			H265_BEGIN_ARRAY_ITEM();

			H265_BITS(st_rps->used_by_curr_pic_flag[j], 1);

			if (!st_rps->used_by_curr_pic_flag[j])
				H265_BITS(st_rps->use_delta_flag[j], 1);

			H265_END_ARRAY_ITEM();
		}
		H265_END_ARRAY(delta_pocs);

		/*
		 * We have to apply the derivation process present in 7.4.8,
		 * after the semantics of use_delta_flag, to handle the parsing
		 * of future rps correctly. Be wary that DeltaPocS* and
		 * delta_poc_s*_minus1 have surprisingly different semantics:
		 * the expected DeltaPocS*[i] == delta_poc_s*_minus1[i] + 1 is
		 * not true at all.
		 */

		uint32_t i = 0;
		for (int j = ref_rps->num_positive_pics - 1; j >= 0; --j) {
			int32_t d_poc =
				ref_rps->derived_delta_poc_s1[j] + delta_rps;

			uint32_t idx = ref_rps->num_negative_pics + j;
			if (d_poc < 0 && st_rps->use_delta_flag[idx]) {
				st_rps->derived_delta_poc_s0[i] = d_poc;
				st_rps->used_by_curr_pic_s0_flag[i++] =
					st_rps->used_by_curr_pic_flag[idx];
			}
		}
		if (delta_rps < 0 && st_rps->use_delta_flag[num_delta_pocs]) {
			st_rps->derived_delta_poc_s0[i] = delta_rps;
			st_rps->used_by_curr_pic_s0_flag[i++] =
				st_rps->used_by_curr_pic_flag[num_delta_pocs];
		}
		for (uint32_t j = 0; j < ref_rps->num_negative_pics; ++j) {
			int32_t d_poc =
				ref_rps->derived_delta_poc_s0[j] + delta_rps;
			if (d_poc < 0 && st_rps->use_delta_flag[j]) {
				st_rps->derived_delta_poc_s0[i] = d_poc;
				st_rps->used_by_curr_pic_s0_flag[i++] =
					st_rps->used_by_curr_pic_flag[j];
			}
		}
		st_rps->num_negative_pics = i;

		i = 0;
		for (int j = ref_rps->num_negative_pics - 1; j >= 0; --j) {
			int32_t d_poc =
				ref_rps->derived_delta_poc_s0[j] + delta_rps;
			if (d_poc > 0 && st_rps->use_delta_flag[j]) {
				st_rps->derived_delta_poc_s1[i] = d_poc;
				st_rps->used_by_curr_pic_s1_flag[i++] =
					st_rps->used_by_curr_pic_flag[j];
			}
		}
		if (delta_rps > 0 && st_rps->use_delta_flag[num_delta_pocs]) {
			st_rps->derived_delta_poc_s1[i] = delta_rps;
			st_rps->used_by_curr_pic_s1_flag[i++] =
				st_rps->used_by_curr_pic_flag[num_delta_pocs];
		}
		for (uint32_t j = 0; j < ref_rps->num_positive_pics; ++j) {
			int32_t d_poc =
				ref_rps->derived_delta_poc_s1[j] + delta_rps;

			uint32_t idx = ref_rps->num_negative_pics + j;
			if (d_poc > 0 && st_rps->use_delta_flag[idx]) {
				st_rps->derived_delta_poc_s1[i] = d_poc;
				st_rps->used_by_curr_pic_s1_flag[i++] =
					st_rps->used_by_curr_pic_flag[idx];
			}
		}
		st_rps->num_positive_pics = i;
	} else {
		H265_BITS_UE(st_rps->num_negative_pics);
		H265_BITS_UE(st_rps->num_positive_pics);

		H265_BEGIN_ARRAY(negative_pics);
		for (uint32_t i = 0; i < st_rps->num_negative_pics; ++i) {
			H265_BEGIN_ARRAY_ITEM();

			H265_BITS_UE(st_rps->delta_poc_s0_minus1[i]);
			H265_BITS(st_rps->used_by_curr_pic_s0_flag[i], 1);

			H265_END_ARRAY_ITEM();
		}
		H265_END_ARRAY(negative_pics);

		H265_BEGIN_ARRAY(positive_pics);
		for (uint32_t i = 0; i < st_rps->num_positive_pics; ++i) {
			H265_BEGIN_ARRAY_ITEM();

			H265_BITS_UE(st_rps->delta_poc_s1_minus1[i]);
			H265_BITS(st_rps->used_by_curr_pic_s1_flag[i], 1);

			H265_END_ARRAY_ITEM();
		}
		H265_END_ARRAY(positive_pics);

		/**
		 * Deriving these variables is necessary
		 */
		st_rps->derived_delta_poc_s0[0] =
			-(st_rps->delta_poc_s0_minus1[0] + 1);
		for (uint32_t i = 1; i < st_rps->num_negative_pics; ++i)
			st_rps->derived_delta_poc_s0[i] =
				st_rps->derived_delta_poc_s0[i - 1] -
				(st_rps->delta_poc_s0_minus1[i] + 1);

		st_rps->derived_delta_poc_s1[0] =
			st_rps->delta_poc_s1_minus1[0] + 1;
		for (uint32_t i = 1; i < st_rps->num_positive_pics; ++i)
			st_rps->derived_delta_poc_s1[i] =
				st_rps->derived_delta_poc_s1[i - 1] +
				st_rps->delta_poc_s1_minus1[i] + 1;
	}

	return 0;
}


static int H265_SYNTAX_FCT(vui)(struct h265_bitstream *bs,
				uint32_t max_sub_layers_minus1,
				struct h265_vui *vui)
{
	int res = 0;

	H265_BITS(vui->aspect_ratio_info_present_flag, 1);
	if (vui->aspect_ratio_info_present_flag) {
		H265_BITS(vui->aspect_ratio_idc, 8);
		if (vui->aspect_ratio_idc == H265_ASPECT_RATIO_EXTENDED_SAR) {
			H265_BITS(vui->sar_width, 16);
			H265_BITS(vui->sar_height, 16);
		}
	}

	H265_BITS(vui->overscan_info_present_flag, 1);
	if (vui->overscan_info_present_flag)
		H265_BITS(vui->overscan_appropriate_flag, 1);

	H265_BITS(vui->video_signal_type_present_flag, 1);
	if (vui->video_signal_type_present_flag) {
		H265_BITS(vui->video_format, 3);
		H265_BITS(vui->video_full_range_flag, 1);

		H265_BITS(vui->colour_description_present_flag, 1);
		if (vui->colour_description_present_flag) {
			H265_BITS(vui->colour_primaries, 8);
			H265_BITS(vui->transfer_characteristics, 8);
			H265_BITS(vui->matrix_coeffs, 8);
		}
	}

	H265_BITS(vui->chroma_loc_info_present_flag, 1);
	if (vui->chroma_loc_info_present_flag) {
		H265_BITS_UE(vui->chroma_sample_loc_type_top_field);
		H265_BITS_UE(vui->chroma_sample_loc_type_bottom_field);
	}

	H265_BITS(vui->neutral_chroma_indication_flag, 1);
	H265_BITS(vui->field_seq_flag, 1);
	H265_BITS(vui->frame_field_info_present_flag, 1);

	H265_BITS(vui->default_display_window_flag, 1);
	if (vui->default_display_window_flag) {
		H265_BITS_UE(vui->def_disp_win_left_offset);
		H265_BITS_UE(vui->def_disp_win_right_offset);
		H265_BITS_UE(vui->def_disp_win_top_offset);
		H265_BITS_UE(vui->def_disp_win_bottom_offset);
	}

	H265_BITS(vui->vui_timing_info_present_flag, 1);
	if (vui->vui_timing_info_present_flag) {
		H265_BITS(vui->vui_num_units_in_tick, 32);
		H265_BITS(vui->vui_time_scale, 32);

		H265_BITS(vui->vui_poc_proportional_to_timing_flag, 1);
		if (vui->vui_poc_proportional_to_timing_flag)
			H265_BITS_UE(vui->vui_num_ticks_poc_diff_one_minus1);

		H265_BITS(vui->vui_hrd_parameters_present_flag, 1);
		if (vui->vui_hrd_parameters_present_flag) {
			H265_BEGIN_STRUCT(hrd);
			res = H265_SYNTAX_FCT(hrd)(
				bs, 1, max_sub_layers_minus1, &vui->hrd);

			ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
			H265_END_STRUCT(hrd);
		}
	}

	H265_BITS(vui->bitstream_restriction_flag, 1);
	if (vui->bitstream_restriction_flag) {
		H265_BITS(vui->tiles_fixed_structure_flag, 1);
		H265_BITS(vui->motion_vectors_over_pic_boundaries_flag, 1);
		H265_BITS(vui->restricted_ref_pic_lists_flag, 1);
		H265_BITS_UE(vui->min_spatial_segmentation_idc);
		H265_BITS_UE(vui->max_bytes_per_pic_denom);
		H265_BITS_UE(vui->max_bits_per_min_cu_denom);
		H265_BITS_UE(vui->log2_max_mv_length_horizontal);
		H265_BITS_UE(vui->log2_max_mv_length_vertical);
	}

	return 0;
}


static int H265_SYNTAX_FCT(sps_range_ext)(struct h265_bitstream *bs,
					  struct h265_sps_range_ext *ext)
{
	H265_BITS(ext->transform_skip_rotation_enabled_flag, 1);
	H265_BITS(ext->transform_skip_context_enabled_flag, 1);
	H265_BITS(ext->implicit_rdpcm_enabled_flag, 1);
	H265_BITS(ext->explicit_rdpcm_enabled_flag, 1);
	H265_BITS(ext->extended_precision_processing_flag, 1);
	H265_BITS(ext->intra_smoothing_disabled_flag, 1);
	H265_BITS(ext->high_precision_offsets_enabled_flag, 1);
	H265_BITS(ext->persistent_rice_adaptation_enabled_flag, 1);
	H265_BITS(ext->cabac_bypass_alignment_enabled_flag, 1);

	return 0;
}


static int
	H265_SYNTAX_FCT(sps_multilayer_ext)(struct h265_bitstream *bs,
					    struct h265_sps_multilayer_ext *ext)
{
	H265_BITS(ext->inter_view_mv_vert_constraint_flag, 1);

	return 0;
}


static int H265_SYNTAX_FCT(sps_3d_ext)(struct h265_bitstream *bs,
				       struct h265_sps_3d_ext *ext)
{
	for (uint32_t d = 0; d <= 1; ++d) {
		H265_BITS(ext->iv_di_mc_enabled_flag[d], 1);
		H265_BITS(ext->iv_mv_scal_enabled_flag[d], 1);

		if (d == 0) {
			H265_BITS_UE(ext->log2_ivmc_sub_pb_size_minus3[d]);
			H265_BITS(ext->iv_res_pred_enabled_flag[d], 1);
			H265_BITS(ext->depth_ref_enabled_flag[d], 1);
			H265_BITS(ext->vsp_mc_enabled_flag[d], 1);
			H265_BITS(ext->dbbp_enabled_flag[d], 1);
		} else {
			H265_BITS(ext->tex_mc_enabled_flag[d], 1);
			H265_BITS_UE(ext->log2_texmc_sub_pb_size_minus3[d]);
			H265_BITS(ext->intra_contour_enabled_flag[d], 1);
			H265_BITS(ext->intra_dc_only_wedge_enabled_flag[d], 1);
			H265_BITS(ext->cqt_cu_part_pred_enabled_flag[d], 1);
			H265_BITS(ext->inter_dc_only_enabled_flag[d], 1);
			H265_BITS(ext->skip_intra_enabled_flag[d], 1);
		}
	}

	return 0;
}


static int H265_SYNTAX_FCT(scc_comps)(struct h265_bitstream *bs,
				      uint32_t chroma_format_idc,
				      uint32_t bit_depth_luma_minus8,
				      uint32_t bit_depth_chroma_minus8,
				      struct h265_sps_scc_ext *ext)
{
	uint32_t num_comps = chroma_format_idc == 0 ? 1 : 3;

	H265_BEGIN_ARRAY(comps);
	for (uint32_t comp = 0; comp < num_comps; ++comp) {
		H265_BEGIN_ARRAY_ITEM();

		H265_BEGIN_ARRAY(palette_predictors);
		for (uint32_t i = 0;
		     i <= ext->sps_num_palette_predictor_initializer_minus1;
		     ++i) {
			H265_BEGIN_ARRAY_ITEM();

/* Silence warning for the dump instantiation */
#if H265_SYNTAX_OP_KIND != H265_SYNTAX_OP_KIND_DUMP
			uint32_t bit_depth =
				(comp == 0 ? bit_depth_luma_minus8
					   : bit_depth_chroma_minus8) +
				8;
#endif

			H265_BITS(ext->sps_palette_predictor_initializers[comp]
									 [i],
				  bit_depth);

			H265_END_ARRAY_ITEM();
		}
		H265_END_ARRAY(palette_predictors);

		H265_END_ARRAY_ITEM();
	}
	H265_END_ARRAY(comps);

	return 0;
}


static int H265_SYNTAX_FCT(sps_scc_ext)(struct h265_bitstream *bs,
					uint32_t chroma_format_idc,
					uint32_t bit_depth_luma_minus8,
					uint32_t bit_depth_chroma_minus8,
					struct h265_sps_scc_ext *ext)
{
	int res = 0;

	H265_BITS(ext->sps_curr_pic_ref_enabled_flag, 1);

	uint32_t *num_predictor_alias =
		&ext->sps_num_palette_predictor_initializer_minus1;

	H265_BITS(ext->palette_mode_enabled_flag, 1);
	if (ext->palette_mode_enabled_flag) {
		H265_BITS_UE(ext->palette_max_size);
		H265_BITS_UE(ext->delta_palette_max_predictor_size);

		H265_BITS(ext->sps_palette_predictor_initializer_present_flag,
			  1);
		if (ext->sps_palette_predictor_initializer_present_flag) {
			H265_BITS_UE(*num_predictor_alias);

			res = H265_SYNTAX_FCT(scc_comps)(
				bs,
				chroma_format_idc,
				bit_depth_luma_minus8,
				bit_depth_chroma_minus8,
				ext);

			ULOG_ERRNO_RETURN_ERR_IF(res > 0, -res);
		}
	}

	H265_BITS(ext->motion_vector_resolution_control_idc, 2);
	H265_BITS(ext->intra_boundary_filtering_disabled_flag, 1);

	return 0;
}


static int H265_SYNTAX_FCT(sps)(struct h265_bitstream *bs, struct h265_sps *sps)
{
	int res = 0;

	H265_BITS(sps->sps_video_parameter_set_id, 4);
	H265_BITS(sps->sps_max_sub_layers_minus1, 3);
	H265_BITS(sps->sps_temporal_id_nesting_flag, 1);

	H265_BEGIN_STRUCT(profile_tier_level);
	res = H265_SYNTAX_FCT(profile_tier_level)(
		bs,
		1,
		sps->sps_max_sub_layers_minus1,
		&sps->profile_tier_level);
	H265_END_STRUCT(profile_tier_level);

	ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

	H265_BITS_UE(sps->sps_seq_parameter_set_id);
	H265_BITS_UE(sps->chroma_format_idc);

	if (sps->chroma_format_idc == 3)
		H265_BITS(sps->separate_colour_plane_flag, 1);

	H265_BITS_UE(sps->pic_width_in_luma_samples);
	H265_BITS_UE(sps->pic_height_in_luma_samples);

	H265_BITS(sps->conformance_window_flag, 1);
	if (sps->conformance_window_flag) {
		H265_BITS_UE(sps->conf_win_left_offset);
		H265_BITS_UE(sps->conf_win_right_offset);
		H265_BITS_UE(sps->conf_win_top_offset);
		H265_BITS_UE(sps->conf_win_bottom_offset);
	}

	H265_BITS_UE(sps->bit_depth_luma_minus8);
	H265_BITS_UE(sps->bit_depth_chroma_minus8);
	H265_BITS_UE(sps->log2_max_pic_order_cnt_lsb_minus4);
	H265_BITS(sps->sps_sub_layer_ordering_info_present_flag, 1);

	uint32_t i = sps->sps_sub_layer_ordering_info_present_flag
			     ? 0
			     : sps->sps_max_sub_layers_minus1;

	H265_BEGIN_ARRAY(sub_layers);
	for (; i <= sps->sps_max_sub_layers_minus1; ++i) {
		H265_BEGIN_ARRAY_ITEM();

		H265_BITS_UE(sps->sps_max_dec_pic_buffering_minus1[i]);
		H265_BITS_UE(sps->sps_max_num_reorder_pics[i]);
		H265_BITS_UE(sps->sps_max_latency_increase_plus1[i]);

		H265_END_ARRAY_ITEM();
	}
	H265_END_ARRAY(sub_layers);

	H265_BITS_UE(sps->log2_min_luma_coding_block_size_minus3);
	H265_BITS_UE(sps->log2_diff_max_min_luma_coding_block_size);
	H265_BITS_UE(sps->log2_min_luma_transform_block_size_minus2);
	H265_BITS_UE(sps->log2_diff_max_min_luma_transform_block_size);
	H265_BITS_UE(sps->max_transform_hierarchy_depth_inter);
	H265_BITS_UE(sps->max_transform_hierarchy_depth_intra);

	H265_BITS(sps->scaling_list_enabled_flag, 1);
	if (sps->scaling_list_enabled_flag) {
		H265_BITS(sps->sps_scaling_list_data_present_flag, 1);
		if (sps->sps_scaling_list_data_present_flag) {
			H265_BEGIN_STRUCT(scaling_list_data);
			res = H265_SYNTAX_FCT(scaling_list_data)(
				bs, &sps->scaling_list_data);
			H265_END_STRUCT(scaling_list_data);

			ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
		}
	}

	H265_BITS(sps->amp_enabled_flag, 1);
	H265_BITS(sps->sample_adaptive_offset_enabled_flag, 1);

	H265_BITS(sps->pcm_enabled_flag, 1);
	if (sps->pcm_enabled_flag) {
		H265_BITS(sps->pcm_sample_bit_depth_luma_minus1, 4);
		H265_BITS(sps->pcm_sample_bit_depth_chroma_minus1, 4);
		H265_BITS_UE(sps->log2_min_pcm_luma_coding_block_size_minus3);
		H265_BITS_UE(sps->log2_diff_max_min_pcm_luma_coding_block_size);
		H265_BITS(sps->pcm_loop_filter_disabled_flag, 1);
	}

	H265_BITS_UE(sps->num_short_term_ref_pic_sets);

	H265_BEGIN_ARRAY(st_ref_pic_sets);
	for (uint32_t i = 0; i < sps->num_short_term_ref_pic_sets; ++i) {
		H265_BEGIN_ARRAY_ITEM();

		H265_BEGIN_STRUCT(st_ref_pic_set);
		res = H265_SYNTAX_FCT(st_ref_pic_set)(
			bs,
			i,
			sps->num_short_term_ref_pic_sets,
			sps->st_ref_pic_sets);
		H265_END_STRUCT(st_ref_pic_set);

		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

		H265_END_ARRAY_ITEM();
	}
	H265_END_ARRAY(st_ref_pic_sets);

	H265_BITS(sps->long_term_ref_pics_present_flag, 1);
	if (sps->long_term_ref_pics_present_flag) {
		H265_BITS_UE(sps->num_long_term_ref_pics_sps);

		H265_BEGIN_ARRAY(long_term_ref_pics_sps);
		for (uint32_t i = 0; i < sps->num_long_term_ref_pics_sps; ++i) {
			H265_BEGIN_ARRAY_ITEM();

#if H265_SYNTAX_OP_KIND != H265_SYNTAX_OP_KIND_DUMP
			uint32_t num_bits =
				sps->log2_max_pic_order_cnt_lsb_minus4 + 4;
#endif

			H265_BITS(sps->lt_ref_pic_poc_lsb_sps[i], num_bits);
			H265_BITS(sps->used_by_curr_pic_lt_sps_flag[i], 1);

			H265_END_ARRAY_ITEM();
		}
		H265_END_ARRAY(long_term_ref_pics_sps);
	}

	H265_BITS(sps->sps_temporal_mvp_enabled_flag, 1);
	H265_BITS(sps->strong_intra_smoothing_enabled_flag, 1);

	H265_BITS(sps->vui_parameters_present_flag, 1);
	if (sps->vui_parameters_present_flag) {
		H265_BEGIN_STRUCT(vui);
		res = H265_SYNTAX_FCT(vui)(
			bs, sps->sps_max_sub_layers_minus1, &sps->vui);
		H265_END_STRUCT(vui);

		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
	}

	H265_BITS(sps->sps_extension_present_flag, 1);
	if (sps->sps_extension_present_flag) {
		H265_BITS(sps->sps_range_extension_flag, 1);
		H265_BITS(sps->sps_multilayer_extension_flag, 1);
		H265_BITS(sps->sps_3d_extension_flag, 1);
		H265_BITS(sps->sps_scc_extension_flag, 1);
		H265_BITS(sps->sps_extension_4bits, 4);
	}

	if (sps->sps_range_extension_flag) {
		H265_BEGIN_STRUCT(sps_range_ext);
		res = H265_SYNTAX_FCT(sps_range_ext)(bs, &sps->sps_range_ext);
		H265_END_STRUCT(sps_range_ext);

		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
	}

	if (sps->sps_multilayer_extension_flag) {
		H265_BEGIN_STRUCT(sps_multilayer_set);
		res = H265_SYNTAX_FCT(sps_multilayer_ext)(
			bs, &sps->sps_multilayer_ext);
		H265_END_STRUCT(sps_multilayer_set);

		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
	}

	if (sps->sps_3d_extension_flag) {
		H265_BEGIN_STRUCT(sps_3d_ext);
		res = H265_SYNTAX_FCT(sps_3d_ext)(bs, &sps->sps_3d_ext);
		H265_END_STRUCT(sps_3d_ext);

		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
	}

	if (sps->sps_scc_extension_flag) {
		H265_BEGIN_STRUCT(sps_scc_ext);
		res = H265_SYNTAX_FCT(sps_scc_ext)(bs,
						   sps->chroma_format_idc,
						   sps->bit_depth_luma_minus8,
						   sps->bit_depth_chroma_minus8,
						   &sps->sps_scc_ext);
		H265_END_STRUCT(sps_scc_ext);

		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
	}

	/* TODO: sps_extension_data_flag */

	H265_BITS_RBSP_TRAILING();

	return 0;
}


static int H265_SYNTAX_FCT(delta_dlt)(struct h265_bitstream *bs,
				      uint32_t bit_depth,
				      struct h265_delta_dlt *dlt)
{
	H265_BITS(dlt->num_val_delta_dlt, bit_depth);

	if (dlt->num_val_delta_dlt > 0) {
		if (dlt->num_val_delta_dlt > 1)
			H265_BITS(dlt->max_diff, bit_depth);

		if (dlt->num_val_delta_dlt > 2 && dlt->max_diff > 0)
			H265_BITS(dlt->min_diff_minus1, bit_depth);

		H265_BITS(dlt->delta_dlt_val0, bit_depth);

		if (dlt->max_diff > (dlt->min_diff_minus1 + 1)) {

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ

			dlt->delta_val_diff_minus_min = calloc(
				dlt->num_val_delta_dlt, sizeof(uint32_t));
			ULOG_ERRNO_RETURN_ERR_IF(
				dlt->delta_val_diff_minus_min == NULL, ENOMEM);

#endif

#if H265_SYNTAX_OP_KIND != H265_SYNTAX_OP_KIND_DUMP
			uint32_t val_bit_length = ceil(
				log2(dlt->max_diff - dlt->min_diff_minus1));
#endif

			H265_BEGIN_ARRAY(val_delta_dlt);
			for (uint32_t k = 1; k < dlt->num_val_delta_dlt; ++k) {
				H265_BEGIN_ARRAY_ITEM();

				H265_BITS(dlt->delta_val_diff_minus_min[k],
					  val_bit_length);

				H265_END_ARRAY_ITEM();
			}
			H265_END_ARRAY(val_delta_dlt);
		}
	}

	return 0;
}


static int H265_SYNTAX_FCT(dlt)(struct h265_bitstream *bs,
				struct h265_pps_3d_ext *ext,
				uint32_t i)
{
	int res = 0;

	H265_BITS(ext->dlt_flag[i], 1);
	if (ext->dlt_flag[i]) {
		H265_BITS(ext->dlt_pred_flag[i], 1);
		if (!ext->dlt_pred_flag[i])
			H265_BITS(ext->dlt_val_flags_present_flag[i], 1);

		if (ext->dlt_val_flags_present_flag[i]) {
			uint32_t depth =
				ext->pps_bit_depth_for_depth_layers_minus8 + 8;
			uint32_t depth_max_value = (1 << depth) - 1;

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ

			ext->dlt_value_flag[i] =
				calloc(depth_max_value + 1, sizeof(int));

			ULOG_ERRNO_RETURN_ERR_IF(ext->dlt_value_flag[i] == NULL,
						 ENOMEM);

#endif

			for (uint32_t j = 0; j <= depth_max_value; ++j)
				H265_BITS(ext->dlt_value_flag[i][j], 1);
		} else {
			res = H265_SYNTAX_FCT(delta_dlt)(
				bs,
				ext->pps_bit_depth_for_depth_layers_minus8 + 8,
				&ext->delta_dlt[i]);

			ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
		}
	}

	return 0;
}


static int H265_SYNTAX_FCT(pps_3d_ext)(struct h265_bitstream *bs,
				       struct h265_pps_3d_ext *ext)
{
	int res = 0;

	H265_BITS(ext->dlts_present_flag, 1);
	if (ext->dlts_present_flag) {
		H265_BITS(ext->pps_depth_layers_minus1, 6);
		H265_BITS(ext->pps_bit_depth_for_depth_layers_minus8, 4);

		for (uint32_t i = 0; i <= ext->pps_depth_layers_minus1; ++i) {
			res = H265_SYNTAX_FCT(dlt)(bs, ext, i);

			ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
		}
	}

	return 0;
}


static int H265_SYNTAX_FCT(pps_range_ext)(struct h265_bitstream *bs,
					  int transform_skip_enabled_flag,
					  struct h265_pps_range_ext *ext)
{
	if (transform_skip_enabled_flag)
		H265_BITS_UE(ext->log2_max_transform_skip_block_size_minus2);

	H265_BITS(ext->cross_component_prediction_enabled_flag, 1);

	H265_BITS(ext->chroma_qp_offset_list_enabled_flag, 1);
	if (ext->chroma_qp_offset_list_enabled_flag) {
		H265_BITS_UE(ext->diff_cu_chroma_qp_offset_depth);

		H265_BITS_UE(ext->chroma_qp_offset_list_len_minus1);
		for (uint32_t i = 0; i < ext->chroma_qp_offset_list_len_minus1;
		     ++i) {
			H265_BITS_SE(ext->cb_qp_offset_list[i]);
			H265_BITS_SE(ext->cr_qp_offset_list[i]);
		}
	}

	H265_BITS_UE(ext->log2_sao_offset_scale_luma);
	H265_BITS_UE(ext->log2_sao_offset_scale_chroma);

	return 0;
}


static int H265_SYNTAX_FCT(pps_palette)(struct h265_bitstream *bs,
					struct h265_pps_scc_ext *ext)
{
	H265_BITS(ext->monochrome_palette_flag, 1);
	H265_BITS_UE(ext->luma_bit_depth_entry_minus8);

	if (!ext->monochrome_palette_flag)
		H265_BITS_UE(ext->chroma_bit_depth_entry_minus8);

	uint32_t num_comps = ext->monochrome_palette_flag ? 1 : 3;

	H265_BEGIN_ARRAY(comps);
	for (uint32_t comp = 0; comp < num_comps; ++comp) {
		H265_BEGIN_ARRAY_ITEM();

		H265_BEGIN_ARRAY(initializers);
		for (uint32_t i = 0;
		     i < ext->pps_num_palette_predictor_initializers;
		     ++i) {
			/* Absence of H265_*_ARRAY_ITEM intentional */

#if H265_SYNTAX_OP_KIND != H265_SYNTAX_OP_KIND_DUMP
			uint32_t bit_length =
				(comp == 0
					 ? ext->luma_bit_depth_entry_minus8
					 : ext->chroma_bit_depth_entry_minus8) +
				8;
#endif


			H265_BITS(
				ext->pps_palette_predictor_initializer[comp][i],
				bit_length);
		}
		H265_END_ARRAY(initializers);

		H265_END_ARRAY_ITEM();
	}
	H265_END_ARRAY(comps);

	return 0;
}


static int H265_SYNTAX_FCT(pps_scc_ext)(struct h265_bitstream *bs,
					struct h265_pps_scc_ext *ext)
{
	int res = 0;

	H265_BITS(ext->pps_curr_pic_ref_enabled_flag, 1);

	H265_BITS(ext->residual_adaptive_colour_transform_enabled_flag, 1);
	if (ext->residual_adaptive_colour_transform_enabled_flag) {
		H265_BITS(ext->pps_slice_act_qp_offsets_present_flag, 1);
		H265_BITS_SE(ext->pps_act_y_qp_offset_plus5);
		H265_BITS_SE(ext->pps_act_cb_qp_offset_plus5);
		H265_BITS_SE(ext->pps_act_cr_qp_offset_plus3);
	}

	H265_BITS(ext->pps_palette_predictor_initializers_present_flag, 1);
	if (ext->pps_palette_predictor_initializers_present_flag) {
		H265_BITS_UE(ext->pps_num_palette_predictor_initializers);
		if (ext->pps_num_palette_predictor_initializers > 0) {
			res = H265_SYNTAX_FCT(pps_palette)(bs, ext);

			ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
		}
	}

	return 0;
}


static int H265_SYNTAX_FCT(pps)(struct h265_bitstream *bs, struct h265_pps *pps)
{
	int res = 0;

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
	/* Fields with non-zero default values */
	pps->uniform_spacing_flag = 1;
#endif

	H265_BITS_UE(pps->pps_pic_parameter_set_id);
	H265_BITS_UE(pps->pps_seq_parameter_set_id);

	H265_BITS(pps->dependent_slice_segments_enabled_flag, 1);
	H265_BITS(pps->output_flag_present_flag, 1);
	H265_BITS(pps->num_extra_slice_header_bits, 3);
	H265_BITS(pps->sign_data_hiding_enabled_flag, 1);
	H265_BITS(pps->cabac_init_present_flag, 1);
	H265_BITS_UE(pps->num_ref_idx_l0_default_active_minus1);
	H265_BITS_UE(pps->num_ref_idx_l1_default_active_minus1);
	H265_BITS_SE(pps->init_qp_minus26);
	H265_BITS(pps->constrained_intra_pred_flag, 1);
	H265_BITS(pps->transform_skip_enabled_flag, 1);

	H265_BITS(pps->cu_qp_delta_enabled_flag, 1);
	if (pps->cu_qp_delta_enabled_flag)
		H265_BITS_UE(pps->diff_cu_qp_delta_depth);

	H265_BITS_SE(pps->pps_cb_qp_offset);
	H265_BITS_SE(pps->pps_cr_qp_offset);
	H265_BITS(pps->pps_slice_chroma_qp_offsets_present_flag, 1);
	H265_BITS(pps->weighted_pred_flag, 1);
	H265_BITS(pps->weighted_bipred_flag, 1);
	H265_BITS(pps->transquant_bypass_enabled_flag, 1);
	H265_BITS(pps->tiles_enabled_flag, 1);
	H265_BITS(pps->entropy_coding_sync_enabled_flag, 1);

	if (pps->tiles_enabled_flag) {
		H265_BITS_UE(pps->num_tile_columns_minus1);

		H265_BITS_UE(pps->num_tile_rows_minus1);

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
		pps->column_width_minus1 =
			malloc(sizeof(*pps->column_width_minus1) *
			       pps->num_tile_columns_minus1);
		ULOG_ERRNO_RETURN_ERR_IF(pps->column_width_minus1 == NULL,
					 ENOMEM);

		pps->row_height_minus1 =
			malloc(sizeof(*pps->row_height_minus1) *
			       pps->num_tile_rows_minus1);
		ULOG_ERRNO_RETURN_ERR_IF(pps->row_height_minus1 == NULL,
					 ENOMEM);
#endif

		H265_BITS(pps->uniform_spacing_flag, 1);
		if (!pps->uniform_spacing_flag) {
			H265_BEGIN_ARRAY(tile_columns);
			for (uint32_t i = 0; i < pps->num_tile_columns_minus1;
			     ++i) {
				H265_BEGIN_ARRAY_ITEM();

				H265_BITS_UE(pps->column_width_minus1[i]);

				H265_END_ARRAY_ITEM();
			}
			H265_END_ARRAY(tile_columns);

			H265_BEGIN_ARRAY(tile_rows);
			for (uint32_t i = 0; i < pps->num_tile_rows_minus1;
			     ++i) {
				H265_BEGIN_ARRAY_ITEM();

				H265_BITS_UE(pps->row_height_minus1[i]);

				H265_END_ARRAY_ITEM();
			}
			H265_END_ARRAY(tile_rows);
		}
		H265_BITS(pps->loop_filter_across_tiles_enabled_flag, 1);
	}

	H265_BITS(pps->pps_loop_filter_across_slices_enabled_flag, 1);

	H265_BITS(pps->deblocking_filter_control_present_flag, 1);
	if (pps->deblocking_filter_control_present_flag) {
		H265_BITS(pps->deblocking_filter_override_enabled_flag, 1);

		H265_BITS(pps->pps_deblocking_filter_disabled_flag, 1);
		if (!pps->pps_deblocking_filter_disabled_flag) {
			H265_BITS_SE(pps->pps_beta_offset_div2);
			H265_BITS_SE(pps->pps_tc_offset_div2);
		}
	}

	H265_BITS(pps->pps_scaling_list_data_present_flag, 1);
	if (pps->pps_scaling_list_data_present_flag) {
		H265_BEGIN_STRUCT(scaling_list_data);
		res = H265_SYNTAX_FCT(scaling_list_data)(
			bs, &pps->scaling_list_data);
		H265_END_STRUCT(scaling_list_data);

		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
	}

	H265_BITS(pps->lists_modification_present_flag, 1);
	H265_BITS_UE(pps->log2_parallel_merge_level_minus2);
	H265_BITS(pps->slice_segment_header_extension_present_flag, 1);

	H265_BITS(pps->pps_extension_present_flag, 1);
	if (pps->pps_extension_present_flag) {
		H265_BITS(pps->pps_range_extension_flag, 1);
		H265_BITS(pps->pps_multilayer_extension_flag, 1);
		H265_BITS(pps->pps_3d_extension_flag, 1);
		H265_BITS(pps->pps_scc_extension_flag, 1);
		H265_BITS(pps->pps_extension_4bits, 4);
	}

	if (pps->pps_range_extension_flag) {
		H265_BEGIN_STRUCT(pps_range_ext);
		res = H265_SYNTAX_FCT(pps_range_ext)(
			bs,
			pps->transform_skip_enabled_flag,
			&pps->pps_range_ext);
		H265_END_STRUCT(pps_range_ext);

		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
	}

	if (pps->pps_multilayer_extension_flag) {
		res = -EPROTO;
		ULOG_ERRNO("the PPS multilayer extension is not supported",
			   -res);
		return res;
	}

	if (pps->pps_3d_extension_flag) {
		H265_BEGIN_STRUCT(pps_3d_ext);
		res = H265_SYNTAX_FCT(pps_3d_ext)(bs, &pps->pps_3d_ext);
		H265_END_STRUCT(pps_3d_ext);

		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
	}

	if (pps->pps_scc_extension_flag) {
		H265_BEGIN_STRUCT(pps_scc_ext);
		res = H265_SYNTAX_FCT(pps_scc_ext)(bs, &pps->pps_scc_ext);
		H265_END_STRUCT(pps_scc_ext);

		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
	}

	/* TODO: pps_extension_data_flag */

	H265_BITS_RBSP_TRAILING();


	return 0;
}


/**
 * 7.4.2.2
 * Table 7 - 1
 */
static int is_vcl(struct h265_nalu_header *header)
{
	return header->nal_unit_type < 32;
}


/**
 * 7.4.2.4.5
 */
static int
is_first_vcl(struct h265_nalu_header *header, const uint8_t *buf, size_t len)
{
	if (!is_vcl(header) || len == 0)
		return 0;

	/**
	 * Rationale in 7.3.6.1: first_slice_segment_in_pic_flag is the first
	 * bit of the slice header.
	 */
	int first_slice_segment_in_pic = buf[2] >> 7;

	return first_slice_segment_in_pic;
}


/**
 * 7.4.2.4.4
 */
static int
can_start_au(struct h265_nalu_header *header, const uint8_t *buf, size_t len)
{
	if (is_first_vcl(header, buf, len))
		return 1;

	if (header->nuh_layer_id != 0)
		return 0;

	uint32_t ty = header->nal_unit_type;

	return ty == H265_NALU_TYPE_AUD_NUT || ty == H265_NALU_TYPE_VPS_NUT ||
	       ty == H265_NALU_TYPE_SPS_NUT || ty == H265_NALU_TYPE_PPS_NUT ||
	       ty == H265_NALU_TYPE_PREFIX_SEI_NUT || (41 <= ty && ty <= 44) ||
	       (48 <= ty && ty <= 55);
}


static int H265_SYNTAX_FCT(nalu)(struct h265_bitstream *bs,
				 struct h265_ctx *ctx,
				 struct h265_ctx_cbs *cbs,
				 void *userdata)
{
	int res = 0;

	struct h265_nalu_header *header = &ctx->nalu_header;

	const uint8_t *buf = NULL;
	size_t len = 0;

	ctx->nalu_unknown = 0;

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
	buf = bs->cdata + bs->off;
	len = bs->len;
	res = h265_ctx_clear_nalu(ctx);
	ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
#endif

	H265_BEGIN_STRUCT(nalu_header);
	res = H265_SYNTAX_FCT(nalu_header)(bs, header);
	H265_END_STRUCT(nalu_header);

	ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);

	H265_CB(ctx,
		cbs,
		userdata,
		nalu_begin,
		header->nal_unit_type,
		buf,
		len,
		&ctx->nalu_header);

	switch (header->nal_unit_type) {
	case H265_NALU_TYPE_VPS_NUT: {
#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
		struct h265_vps *vps = calloc(1, sizeof(*vps));
		ULOG_ERRNO_RETURN_ERR_IF(vps == NULL, ENOMEM);
#else
		struct h265_vps *vps = ctx->vps;
		ULOG_ERRNO_RETURN_ERR_IF(vps == NULL, EIO);
#endif
		H265_BEGIN_STRUCT(vps);
		res = H265_SYNTAX_FCT(vps)(bs, vps);
		H265_END_STRUCT(vps);

		if (res < 0) {
			ULOG_ERRNO("", -res);
#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
			free(vps);
#endif
			return res;
		}

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
		res = h265_ctx_set_vps(ctx, vps);
		if (res < 0) {
			ULOG_ERRNO("", -res);
			free(vps);
			return res;
		}
		free(vps);
#endif
		H265_CB(ctx, cbs, userdata, vps, buf, len, ctx->vps);
		break;
	}

	case H265_NALU_TYPE_SPS_NUT: {
#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
		struct h265_sps *sps = calloc(1, sizeof(*sps));
		ULOG_ERRNO_RETURN_ERR_IF(sps == NULL, ENOMEM);
#else
		struct h265_sps *sps = ctx->sps;
		ULOG_ERRNO_RETURN_ERR_IF(sps == NULL, EIO);
#endif

		H265_BEGIN_STRUCT(sps);
		res = H265_SYNTAX_FCT(sps)(bs, sps);
		H265_END_STRUCT(sps);

		if (res < 0) {
			ULOG_ERRNO("", -res);
#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
			free(sps);
#endif
			return res;
		}

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
		res = h265_ctx_set_sps(ctx, sps);
		if (res < 0) {
			ULOG_ERRNO("", -res);
			free(sps);
			return res;
		}
		free(sps);
#endif
		H265_CB(ctx, cbs, userdata, sps, buf, len, ctx->sps);
		break;
	}

	case H265_NALU_TYPE_PPS_NUT: {
#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
		struct h265_pps *pps = calloc(1, sizeof(*pps));
		ULOG_ERRNO_RETURN_ERR_IF(pps == NULL, ENOMEM);
#else
		struct h265_pps *pps = ctx->pps;
		ULOG_ERRNO_RETURN_ERR_IF(pps == NULL, EIO);
#endif
		H265_BEGIN_STRUCT(pps);
		res = H265_SYNTAX_FCT(pps)(bs, pps);
		H265_END_STRUCT(pps);

		if (res < 0) {
			ULOG_ERRNO("", -res);
#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
			h265_pps_clear(pps);
			free(pps);
#endif
			return res;
		}

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
		res = h265_ctx_set_pps(ctx, pps);
		if (res < 0) {
			ULOG_ERRNO("", -res);
			h265_pps_clear(pps);
			free(pps);
			return res;
		}
		h265_pps_clear(pps);
		free(pps);
#endif
		H265_CB(ctx, cbs, userdata, pps, buf, len, ctx->pps);
		break;
	}

	case H265_NALU_TYPE_AUD_NUT: {
		H265_BEGIN_STRUCT(aud);
		res = H265_SYNTAX_FCT(aud)(bs, &ctx->aud);
		H265_END_STRUCT(aud);
		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
		H265_CB(ctx, cbs, userdata, aud, buf, len, &ctx->aud);
		break;
	}

	case H265_NALU_TYPE_PREFIX_SEI_NUT: {
		H265_BEGIN_ARRAY(sei);
		res = H265_SYNTAX_FCT(sei)(bs, ctx, cbs, userdata);
		H265_END_ARRAY(sei);
		ULOG_ERRNO_RETURN_ERR_IF(res < 0, -res);
		break;
	}

	default:
		ctx->nalu_unknown = 1;
		/* TODO */
		break;
	}

#if H265_SYNTAX_OP_KIND == H265_SYNTAX_OP_KIND_READ
	/* AU change detection */
	if (ctx->first_vcl_of_current_frame_found &&
	    can_start_au(header, buf, len)) {
		H265_CB(ctx, cbs, userdata, au_end);
		ctx->first_vcl_of_current_frame_found = 0;
	}
	if (is_first_vcl(header, buf, len))
		ctx->first_vcl_of_current_frame_found = 1;
#endif

	H265_CB(ctx,
		cbs,
		userdata,
		nalu_end,
		header->nal_unit_type,
		buf,
		len,
		&ctx->nalu_header);

	return 0;
}


#endif /* _H265_SYNTAX_H_ */
