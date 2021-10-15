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


static int h265_ctx_clear_sei_table(struct h265_ctx *ctx)
{
	ULOG_ERRNO_RETURN_ERR_IF(ctx == NULL, EINVAL);
	for (uint32_t i = 0; i < ctx->sei_count; i++)
		free(ctx->sei_table[i].raw.buf);
	free(ctx->sei_table);
	ctx->sei_table = NULL;
	ctx->sei_count = 0;
	return 0;
}


static void uninit(struct h265_ctx *ctx)
{
	h265_ctx_clear_sei_table(ctx);
	for (size_t i = 0; i < ARRAY_SIZE(ctx->vps_table); ++i)
		free(ctx->vps_table[i]);
	for (size_t i = 0; i < ARRAY_SIZE(ctx->sps_table); ++i)
		free(ctx->sps_table[i]);
	for (size_t i = 0; i < ARRAY_SIZE(ctx->pps_table); ++i) {
		h265_pps_clear(ctx->pps_table[i]);
		free(ctx->pps_table[i]);
	}
}


int h265_ctx_clear_nalu(struct h265_ctx *ctx)
{
	ULOG_ERRNO_RETURN_ERR_IF(ctx == NULL, EINVAL);
	h265_ctx_clear_sei_table(ctx);
	return 0;
}


int h265_ctx_set_nalu_header(struct h265_ctx *ctx,
			     const struct h265_nalu_header *nh)
{
	ULOG_ERRNO_RETURN_ERR_IF(ctx == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(nh == NULL, EINVAL);
	ctx->nalu_header = *nh;
	return 0;
}


int h265_ctx_new(struct h265_ctx **ret_obj)
{
	ULOG_ERRNO_RETURN_ERR_IF(ret_obj == NULL, EINVAL);

	struct h265_ctx *ctx = calloc(1, sizeof(*ctx));
	if (ctx == NULL)
		return -ENOMEM;

	*ret_obj = ctx;
	return 0;
}


int h265_ctx_destroy(struct h265_ctx *ctx)
{
	if (ctx == NULL)
		return 0;

	uninit(ctx);
	free(ctx);

	return 0;
}


int h265_ctx_is_nalu_unknown(struct h265_ctx *ctx)
{
	return ctx == NULL ? 0 : ctx->nalu_unknown;
}


int h265_ctx_set_aud(struct h265_ctx *ctx, const struct h265_aud *aud)
{
	ULOG_ERRNO_RETURN_ERR_IF(ctx == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(aud == NULL, EINVAL);
	ctx->aud = *aud;
	return 0;
}


int h265_ctx_set_vps(struct h265_ctx *ctx, const struct h265_vps *vps)
{
	ULOG_ERRNO_RETURN_ERR_IF(ctx == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(vps == NULL, EINVAL);
	int valid_id =
		vps->vps_video_parameter_set_id < ARRAY_SIZE(ctx->vps_table);
	ULOG_ERRNO_RETURN_ERR_IF(!valid_id, EINVAL);

	struct h265_vps **p_vps =
		&ctx->vps_table[vps->vps_video_parameter_set_id];

	free(*p_vps);
	*p_vps = calloc(1, sizeof(**p_vps));
	if (*p_vps == NULL)
		return -ENOMEM;

	**p_vps = *vps;
	ctx->vps = *p_vps;

	return 0;
}


int h265_ctx_set_sps(struct h265_ctx *ctx, const struct h265_sps *sps)
{
	ULOG_ERRNO_RETURN_ERR_IF(ctx == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(sps == NULL, EINVAL);
	int valid_id =
		sps->sps_seq_parameter_set_id < ARRAY_SIZE(ctx->sps_table);
	ULOG_ERRNO_RETURN_ERR_IF(!valid_id, EINVAL);

	struct h265_sps **p_sps =
		&ctx->sps_table[sps->sps_seq_parameter_set_id];

	free(*p_sps);
	*p_sps = calloc(1, sizeof(**p_sps));
	if (*p_sps == NULL)
		return -ENOMEM;

	**p_sps = *sps;
	ctx->sps = *p_sps;

	return 0;
}


static int pps_3d_ext_copy(const struct h265_pps_3d_ext *src,
			   struct h265_pps_3d_ext *dst,
			   uint32_t i)
{
	int ret;
	dst->dlt_value_flag[i] = NULL;
	dst->delta_dlt[i].delta_val_diff_minus_min = NULL;

	if (src->dlt_val_flags_present_flag[i]) {
		uint32_t depth = src->pps_bit_depth_for_depth_layers_minus8 + 8;
		uint32_t depth_max_value = (1 << depth) - 1;

		dst->dlt_value_flag[i] =
			malloc(sizeof(int) * (depth_max_value + 1));
		if (dst->dlt_value_flag[i] == NULL) {
			ret = -ENOMEM;
			goto error;
		}
		memcpy(dst->dlt_value_flag[i],
		       src->dlt_value_flag[i],
		       sizeof(int) * (depth_max_value + 1));
	} else if ((src->delta_dlt[i].num_val_delta_dlt > 0) &&
		   (src->delta_dlt[i].max_diff >
		    (src->delta_dlt[i].min_diff_minus1 + 1))) {
		dst->delta_dlt[i].delta_val_diff_minus_min = malloc(
			sizeof(uint32_t) * src->delta_dlt[i].num_val_delta_dlt);
		if (dst->delta_dlt[i].delta_val_diff_minus_min == NULL) {
			ret = -ENOMEM;
			goto error;
		}
		memcpy(dst->delta_dlt[i].delta_val_diff_minus_min,
		       src->delta_dlt[i].delta_val_diff_minus_min,
		       sizeof(uint32_t) * src->delta_dlt[i].num_val_delta_dlt);
	}

	return 0;

error:
	free(dst->dlt_value_flag[i]);
	free(dst->delta_dlt[i].delta_val_diff_minus_min);
	return ret;
}


int h265_ctx_set_pps(struct h265_ctx *ctx, const struct h265_pps *pps)
{
	ULOG_ERRNO_RETURN_ERR_IF(ctx == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(pps == NULL, EINVAL);
	int valid_id =
		pps->pps_seq_parameter_set_id < ARRAY_SIZE(ctx->pps_table);
	ULOG_ERRNO_RETURN_ERR_IF(!valid_id, EINVAL);

	int ret;
	struct h265_pps **p_pps =
		&ctx->pps_table[pps->pps_pic_parameter_set_id];

	h265_pps_clear(*p_pps);
	free(*p_pps);

	*p_pps = calloc(1, sizeof(**p_pps));
	if (*p_pps == NULL)
		return -ENOMEM;

	**p_pps = *pps;
	(*p_pps)->column_width_minus1 = NULL;
	(*p_pps)->row_height_minus1 = NULL;

	(*p_pps)->column_width_minus1 =
		malloc(sizeof(*pps->column_width_minus1) *
		       pps->num_tile_columns_minus1);
	if ((*p_pps)->column_width_minus1 == NULL) {
		ret = -ENOMEM;
		goto error;
	}
	memcpy((*p_pps)->column_width_minus1,
	       pps->column_width_minus1,
	       sizeof(*pps->column_width_minus1) *
		       pps->num_tile_columns_minus1);

	(*p_pps)->row_height_minus1 = malloc(sizeof(*pps->row_height_minus1) *
					     pps->num_tile_rows_minus1);
	if ((*p_pps)->row_height_minus1 == NULL) {
		ret = -ENOMEM;
		goto error;
	}
	memcpy((*p_pps)->row_height_minus1,
	       pps->row_height_minus1,
	       sizeof(*pps->row_height_minus1) * pps->num_tile_rows_minus1);

	for (uint32_t i = 0;
	     i <= pps->pps_3d_ext.pps_bit_depth_for_depth_layers_minus8;
	     ++i) {
		if (!pps->pps_3d_ext.dlt_flag[i])
			continue;
		ret = pps_3d_ext_copy(
			&pps->pps_3d_ext, &(*p_pps)->pps_3d_ext, i);
		if (ret < 0)
			goto error;
	}

	ctx->pps = *p_pps;

	return 0;

error:
	if (*p_pps != NULL) {
		free((*p_pps)->column_width_minus1);
		free((*p_pps)->row_height_minus1);
		free(*p_pps);
		*p_pps = NULL;
	}
	return ret;
}


const struct h265_vps *h265_ctx_get_vps(struct h265_ctx *ctx)
{
	return ctx == NULL ? NULL : ctx->vps;
}


const struct h265_sps *h265_ctx_get_sps(struct h265_ctx *ctx)
{
	return ctx == NULL ? NULL : ctx->sps;
}


const struct h265_pps *h265_ctx_get_pps(struct h265_ctx *ctx)
{
	return ctx == NULL ? NULL : ctx->pps;
}


int h265_ctx_add_sei_internal(struct h265_ctx *ctx, struct h265_sei **ret_obj)
{
	struct h265_sei *newtable = NULL;
	struct h265_sei *sei = NULL;

	ULOG_ERRNO_RETURN_ERR_IF(ret_obj == NULL, EINVAL);
	*ret_obj = NULL;
	ULOG_ERRNO_RETURN_ERR_IF(ctx == NULL, EINVAL);

	/* Increase table size */
	newtable = realloc(ctx->sei_table,
			   (ctx->sei_count + 1) * sizeof(*newtable));
	if (newtable == NULL)
		return -ENOMEM;
	ctx->sei_table = newtable;

	/* Setup SEI pointer */
	sei = &ctx->sei_table[ctx->sei_count];
	memset(sei, 0, sizeof(*sei));
	ctx->sei_count++;
	*ret_obj = sei;
	return 0;
}


int h265_ctx_add_sei(struct h265_ctx *ctx, const struct h265_sei *sei)
{
	int res = 0;
	struct h265_bitstream bs;
	struct h265_sei *new_sei = NULL;

	ULOG_ERRNO_RETURN_ERR_IF(ctx == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(sei == NULL, EINVAL);

	/* Setup bitstream (without emulation prevention) */
	h265_bs_init(&bs, NULL, 0, 0);

	/* Allocate new SEI in internal table */
	res = h265_ctx_add_sei_internal(ctx, &new_sei);
	if (res < 0)
		goto error;
	*new_sei = *sei;

	/* Encode SEI payload in dynamic bitstream */
	res = h265_write_one_sei(&bs, ctx, new_sei);
	if (res < 0)
		goto error;

	/* Acquire buffer of bitstream */
	res = h265_bs_acquire_buf(&bs, &new_sei->raw.buf, &new_sei->raw.len);
	if (res < 0)
		goto error;

	/* Update internal buffer of SEI structures */
	res = h265_sei_update_internal_buf(new_sei);
	if (res < 0)
		goto error;

	h265_bs_clear(&bs);
	return 0;

error:
	if (new_sei != NULL) {
		free(new_sei->raw.buf);
		ctx->sei_count--;
	}
	h265_bs_clear(&bs);
	return res;
}


int h265_ctx_get_sei_count(struct h265_ctx *ctx)
{
	ULOG_ERRNO_RETURN_ERR_IF(ctx == NULL, EINVAL);
	return ctx->sei_count;
}


uint64_t h265_ctx_sei_time_code_to_ts(struct h265_ctx *ctx,
				      const struct h265_sei_time_code *sei)
{
	uint64_t clock_timestamp;
	const struct h265_sps *sps;

	ULOG_ERRNO_RETURN_VAL_IF(ctx == NULL, EINVAL, 0);
	ULOG_ERRNO_RETURN_VAL_IF(sei == NULL, EINVAL, 0);

	sps = ctx->sps;
	ULOG_ERRNO_RETURN_VAL_IF(sps->vui.vui_time_scale == 0, EPROTO, 0);
	ULOG_ERRNO_RETURN_VAL_IF(
		sps->vui.vui_num_units_in_tick == 0, EPROTO, 0);

	clock_timestamp =
		(((uint64_t)sei->clock_ts[0].hours_value * 60 +
		  sei->clock_ts[0].minutes_value) *
			 60 +
		 sei->clock_ts[0].seconds_value) *
			sps->vui.vui_time_scale +
		((uint64_t)sei->clock_ts[0].n_frames *
		 ((uint64_t)sps->vui.vui_num_units_in_tick *
		  (1 + (uint64_t)sei->clock_ts[0].units_field_based_flag)));

	if (sei->clock_ts[0].time_offset_value < 0 &&
	    ((uint64_t)-sei->clock_ts[0].time_offset_value > clock_timestamp))
		clock_timestamp = 0;
	else
		clock_timestamp += sei->clock_ts[0].time_offset_value;

	return clock_timestamp;
}


uint64_t h265_ctx_sei_time_code_to_us(struct h265_ctx *ctx,
				      const struct h265_sei_time_code *sei)
{
	uint64_t clock_timestamp, clock_timestamp_us;
	const struct h265_sps *sps;

	ULOG_ERRNO_RETURN_VAL_IF(ctx == NULL, EINVAL, 0);
	ULOG_ERRNO_RETURN_VAL_IF(sei == NULL, EINVAL, 0);

	sps = ctx->sps;
	ULOG_ERRNO_RETURN_VAL_IF(sps->vui.vui_time_scale == 0, EPROTO, 0);
	clock_timestamp = h265_ctx_sei_time_code_to_ts(ctx, sei);

	clock_timestamp_us =
		(clock_timestamp * 1000000 + sps->vui.vui_time_scale / 2) /
		sps->vui.vui_time_scale;

	return clock_timestamp_us;
}


int h265_ctx_get_info(struct h265_ctx *ctx, struct h265_info *info)
{
	ULOG_ERRNO_RETURN_ERR_IF(ctx == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(info == NULL, EINVAL);

	if ((ctx->sps == NULL) || (ctx->pps == NULL) || (ctx->vps == NULL))
		return -EAGAIN;

	return h265_get_info_from_ps(ctx->vps, ctx->sps, ctx->pps, info);
}
