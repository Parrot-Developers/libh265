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


int h265_delta_dlt_clear(struct h265_delta_dlt *dlt)
{
	if (dlt == NULL)
		return 0;

	free(dlt->delta_val_diff_minus_min);

	*dlt = (const struct h265_delta_dlt){0};

	return 0;
}


int h265_pps_3d_ext_clear(struct h265_pps_3d_ext *ext)
{
	int res = 0;

	if (ext == NULL)
		return 0;

	for (uint32_t i = 0; i <= ext->pps_depth_layers_minus1; ++i)
		free(ext->dlt_value_flag[i]);

	for (uint32_t i = 0; i <= ext->pps_depth_layers_minus1; ++i) {
		int x = h265_delta_dlt_clear(&ext->delta_dlt[i]);
		if (x < 0)
			res = x;
	}

	memset(ext, 0, sizeof(*ext));

	return res;
}


int h265_pps_clear(struct h265_pps *pps)
{
	if (pps == NULL)
		return 0;

	free(pps->column_width_minus1);
	free(pps->row_height_minus1);

	h265_pps_3d_ext_clear(&pps->pps_3d_ext);

	memset(pps, 0, sizeof(*pps));

	return 0;
}


int h265_pps_cpy(struct h265_pps *dst_pps, const struct h265_pps *src_pps)
{
	ULOG_ERRNO_RETURN_ERR_IF(src_pps == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(dst_pps == NULL, EINVAL);

	int ret;
	memcpy(dst_pps,
	       src_pps,
	       offsetof(struct h265_pps, column_width_minus1));

	dst_pps->column_width_minus1 = NULL;
	dst_pps->row_height_minus1 = NULL;

	dst_pps->column_width_minus1 =
		malloc(sizeof(*src_pps->column_width_minus1) *
		       src_pps->num_tile_columns_minus1);
	if (dst_pps->column_width_minus1 == NULL) {
		ret = -ENOMEM;
		goto error;
	}
	memcpy(dst_pps->column_width_minus1,
	       src_pps->column_width_minus1,
	       sizeof(*src_pps->column_width_minus1) *
		       src_pps->num_tile_columns_minus1);

	dst_pps->row_height_minus1 =
		malloc(sizeof(*src_pps->row_height_minus1) *
		       src_pps->num_tile_rows_minus1);
	if (dst_pps->row_height_minus1 == NULL) {
		ret = -ENOMEM;
		goto error;
	}
	memcpy(dst_pps->row_height_minus1,
	       src_pps->row_height_minus1,
	       sizeof(*src_pps->row_height_minus1) *
		       src_pps->num_tile_rows_minus1);

	return 0;

error:
	free(dst_pps->column_width_minus1);
	free(dst_pps->row_height_minus1);
	return ret;
}


int h265_pps_cmp(const struct h265_pps *pps1, const struct h265_pps *pps2)
{
	ULOG_ERRNO_RETURN_ERR_IF(pps1 == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(pps2 == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(sizeof(*pps1) != sizeof(*pps2), EINVAL);

	size_t i;

	if (memcmp(pps1,
		   pps2,
		   offsetof(struct h265_pps, column_width_minus1)) != 0)
		return 1;

	for (i = 0; i < pps1->num_tile_rows_minus1; i++)
		if (pps1->row_height_minus1[i] != pps2->row_height_minus1[i])
			return 1;
	for (i = 0; i < pps1->num_tile_columns_minus1; i++)
		if (pps1->column_width_minus1[i] !=
		    pps2->column_width_minus1[i])
			return 1;
	return 0;
}


const char *h265_nalu_type_str(enum h265_nalu_type val)
{
	switch (val) {
	case H265_NALU_TYPE_TRAIL_N:
		return "TRAIL_N";
	case H265_NALU_TYPE_TRAIL_R:
		return "TRAIL_R";
	case H265_NALU_TYPE_TSA_N:
		return "TSA_N";
	case H265_NALU_TYPE_TSA_R:
		return "TSA_R";
	case H265_NALU_TYPE_STSA_N:
		return "STSA_N";
	case H265_NALU_TYPE_STSA_R:
		return "STSA_R";
	case H265_NALU_TYPE_RADL_N:
		return "RADL_N";
	case H265_NALU_TYPE_RADL_R:
		return "RADL_R";
	case H265_NALU_TYPE_RASL_N:
		return "RASL_N";
	case H265_NALU_TYPE_RASL_R:
		return "RASL_R";
	case H265_NALU_TYPE_RSV_VCL_N10:
		return "RSV_VCL_N10";
	case H265_NALU_TYPE_RSV_VCL_R11:
		return "RSV_VCL_R11";
	case H265_NALU_TYPE_RSV_VCL_N12:
		return "RSV_VCL_N12";
	case H265_NALU_TYPE_RSV_VCL_R13:
		return "RSV_VCL_R13";
	case H265_NALU_TYPE_RSV_VCL_N14:
		return "RSV_VCL_N14";
	case H265_NALU_TYPE_RSV_VCL_R15:
		return "RSV_VCL_R15";
	case H265_NALU_TYPE_BLA_W_LP:
		return "BLA_W_LP";
	case H265_NALU_TYPE_BLA_W_RADL:
		return "BLA_W_RADL";
	case H265_NALU_TYPE_BLA_N_LP:
		return "BLA_N_LP";
	case H265_NALU_TYPE_IDR_W_RADL:
		return "IDR_W_RADL";
	case H265_NALU_TYPE_IDR_N_LP:
		return "IDR_N_LP";
	case H265_NALU_TYPE_CRA_NUT:
		return "CRA_NUT";
	case H265_NALU_TYPE_RSV_IRAP_VCL22:
		return "RSV_IRAP_VCL22";
	case H265_NALU_TYPE_RSV_IRAP_VCL23:
		return "RSV_IRAP_VCL23";
	case H265_NALU_TYPE_RSV_VCL24:
		return "RSV_VCL24";
	case H265_NALU_TYPE_RSV_VCL25:
		return "RSV_VCL25";
	case H265_NALU_TYPE_RSV_VCL26:
		return "RSV_VCL26";
	case H265_NALU_TYPE_RSV_VCL27:
		return "RSV_VCL27";
	case H265_NALU_TYPE_RSV_VCL28:
		return "RSV_VCL28";
	case H265_NALU_TYPE_RSV_VCL29:
		return "RSV_VCL29";
	case H265_NALU_TYPE_RSV_VCL30:
		return "RSV_VCL30";
	case H265_NALU_TYPE_RSV_VCL31:
		return "RSV_VCL31";
	case H265_NALU_TYPE_VPS_NUT:
		return "VPS_NUT";
	case H265_NALU_TYPE_SPS_NUT:
		return "SPS_NUT";
	case H265_NALU_TYPE_PPS_NUT:
		return "PPS_NUT";
	case H265_NALU_TYPE_AUD_NUT:
		return "AUD_NUT";
	case H265_NALU_TYPE_EOS_NUT:
		return "EOS_NUT";
	case H265_NALU_TYPE_EOB_NUT:
		return "EOB_NUT";
	case H265_NALU_TYPE_FD_NUT:
		return "FD_NUT";
	case H265_NALU_TYPE_PREFIX_SEI_NUT:
		return "PREFIX_SEI_NUT";
	case H265_NALU_TYPE_SUFFIX_SEI_NUT:
		return "SUFFIX_SEI_NUT";
	case H265_NALU_TYPE_RSV_NVCL41:
		return "RSV_NVCL41";
	case H265_NALU_TYPE_RSV_NVCL42:
		return "RSV_NVCL42";
	case H265_NALU_TYPE_RSV_NVCL43:
		return "RSV_NVCL43";
	case H265_NALU_TYPE_RSV_NVCL44:
		return "RSV_NVCL44";
	case H265_NALU_TYPE_RSV_NVCL45:
		return "RSV_NVCL45";
	case H265_NALU_TYPE_RSV_NVCL46:
		return "RSV_NVCL46";
	case H265_NALU_TYPE_RSV_NVCL47:
		return "RSV_NVCL47";
	case H265_NALU_TYPE_UNSPEC48:
		return "UNSPEC48";
	case H265_NALU_TYPE_UNSPEC49:
		return "UNSPEC49";
	case H265_NALU_TYPE_UNSPEC50:
		return "UNSPEC50";
	case H265_NALU_TYPE_UNSPEC51:
		return "UNSPEC51";
	case H265_NALU_TYPE_UNSPEC52:
		return "UNSPEC52";
	case H265_NALU_TYPE_UNSPEC53:
		return "UNSPEC53";
	case H265_NALU_TYPE_UNSPEC54:
		return "UNSPEC54";
	case H265_NALU_TYPE_UNSPEC55:
		return "UNSPEC55";
	case H265_NALU_TYPE_UNSPEC56:
		return "UNSPEC56";
	case H265_NALU_TYPE_UNSPEC57:
		return "UNSPEC57";
	case H265_NALU_TYPE_UNSPEC58:
		return "UNSPEC58";
	case H265_NALU_TYPE_UNSPEC59:
		return "UNSPEC59";
	case H265_NALU_TYPE_UNSPEC60:
		return "UNSPEC60";
	case H265_NALU_TYPE_UNSPEC61:
		return "UNSPEC61";
	case H265_NALU_TYPE_UNSPEC62:
		return "UNSPEC62";
	case H265_NALU_TYPE_UNSPEC63:
		return "UNSPEC63";
	case H265_NALU_TYPE_UNKNOWN:
	default:
		return "UNKNOWN";
	}
}


const char *h265_sei_type_str(enum h265_sei_type val)
{
	switch (val) {
	case H265_SEI_TYPE_USER_DATA_UNREGISTERED:
		return "USER_DATA_UNREGISTERED";
	case H265_SEI_TYPE_RECOVERY_POINT:
		return "RECOVERY_POINT";
	case H265_SEI_TYPE_TIME_CODE:
		return "TIME_CODE";
	case H265_SEI_TYPE_MASTERING_DISPLAY_COLOUR_VOLUME:
		return "MASTERING_DISPLAY_COLOUR_VOLUME";
	case H265_SEI_TYPE_CONTENT_LIGHT_LEVEL:
		return "CONTENT_LIGHT_LEVEL";
	default:
		return "UNKNOWN";
	}
}


int h265_sei_update_internal_buf(struct h265_sei *sei)
{
	uint32_t start = 0;
	const uint8_t **buf = NULL;
	size_t *len = 0;
	ULOG_ERRNO_RETURN_ERR_IF(sei == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(sei->raw.buf == NULL, EINVAL);
	ULOG_ERRNO_RETURN_ERR_IF(sei->raw.len == 0, EINVAL);

	switch (sei->type) {
	case H265_SEI_TYPE_USER_DATA_UNREGISTERED:
		start = 16;
		buf = &sei->user_data_unregistered.buf;
		len = &sei->user_data_unregistered.len;
		break;

	default:
		break;
	}

	if (buf != NULL && len != NULL) {
		ULOG_ERRNO_RETURN_ERR_IF(sei->raw.len < start, EINVAL);
		*buf = sei->raw.buf + start;
		*len = sei->raw.len - start;
	}
	return 0;
}
