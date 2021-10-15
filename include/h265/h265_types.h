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

#ifndef _H265_TYPES_H_
#define _H265_TYPES_H_

/**
 * Types corresponding to syntax elements are defined after edition 5.0 of ITU-T
 * H.265, released 02/2018.
 */

/**
 * 7.4.2.2 NAL unit header semantics
 */
enum h265_nalu_type {
	H265_NALU_TYPE_UNKNOWN = -1,
	H265_NALU_TYPE_TRAIL_N = 0,
	H265_NALU_TYPE_TRAIL_R = 1,
	H265_NALU_TYPE_TSA_N = 2,
	H265_NALU_TYPE_TSA_R = 3,
	H265_NALU_TYPE_STSA_N = 4,
	H265_NALU_TYPE_STSA_R = 5,
	H265_NALU_TYPE_RADL_N = 6,
	H265_NALU_TYPE_RADL_R = 7,
	H265_NALU_TYPE_RASL_N = 8,
	H265_NALU_TYPE_RASL_R = 9,
	H265_NALU_TYPE_RSV_VCL_N10 = 10,
	H265_NALU_TYPE_RSV_VCL_R11 = 11,
	H265_NALU_TYPE_RSV_VCL_N12 = 12,
	H265_NALU_TYPE_RSV_VCL_R13 = 13,
	H265_NALU_TYPE_RSV_VCL_N14 = 14,
	H265_NALU_TYPE_RSV_VCL_R15 = 15,
	H265_NALU_TYPE_BLA_W_LP = 16,
	H265_NALU_TYPE_BLA_W_RADL = 17,
	H265_NALU_TYPE_BLA_N_LP = 18,
	H265_NALU_TYPE_IDR_W_RADL = 19,
	H265_NALU_TYPE_IDR_N_LP = 20,
	H265_NALU_TYPE_CRA_NUT = 21,
	H265_NALU_TYPE_RSV_IRAP_VCL22 = 22,
	H265_NALU_TYPE_RSV_IRAP_VCL23 = 23,
	H265_NALU_TYPE_RSV_VCL24 = 24,
	H265_NALU_TYPE_RSV_VCL25 = 25,
	H265_NALU_TYPE_RSV_VCL26 = 26,
	H265_NALU_TYPE_RSV_VCL27 = 27,
	H265_NALU_TYPE_RSV_VCL28 = 28,
	H265_NALU_TYPE_RSV_VCL29 = 29,
	H265_NALU_TYPE_RSV_VCL30 = 30,
	H265_NALU_TYPE_RSV_VCL31 = 31,
	H265_NALU_TYPE_VPS_NUT = 32,
	H265_NALU_TYPE_SPS_NUT = 33,
	H265_NALU_TYPE_PPS_NUT = 34,
	H265_NALU_TYPE_AUD_NUT = 35,
	H265_NALU_TYPE_EOS_NUT = 36,
	H265_NALU_TYPE_EOB_NUT = 37,
	H265_NALU_TYPE_FD_NUT = 38,
	H265_NALU_TYPE_PREFIX_SEI_NUT = 39,
	H265_NALU_TYPE_SUFFIX_SEI_NUT = 40,
	H265_NALU_TYPE_RSV_NVCL41 = 41,
	H265_NALU_TYPE_RSV_NVCL42 = 42,
	H265_NALU_TYPE_RSV_NVCL43 = 43,
	H265_NALU_TYPE_RSV_NVCL44 = 44,
	H265_NALU_TYPE_RSV_NVCL45 = 45,
	H265_NALU_TYPE_RSV_NVCL46 = 46,
	H265_NALU_TYPE_RSV_NVCL47 = 47,
	H265_NALU_TYPE_UNSPEC48 = 48,
	H265_NALU_TYPE_UNSPEC49 = 49,
	H265_NALU_TYPE_UNSPEC50 = 50,
	H265_NALU_TYPE_UNSPEC51 = 51,
	H265_NALU_TYPE_UNSPEC52 = 52,
	H265_NALU_TYPE_UNSPEC53 = 53,
	H265_NALU_TYPE_UNSPEC54 = 54,
	H265_NALU_TYPE_UNSPEC55 = 55,
	H265_NALU_TYPE_UNSPEC56 = 56,
	H265_NALU_TYPE_UNSPEC57 = 57,
	H265_NALU_TYPE_UNSPEC58 = 58,
	H265_NALU_TYPE_UNSPEC59 = 59,
	H265_NALU_TYPE_UNSPEC60 = 60,
	H265_NALU_TYPE_UNSPEC61 = 61,
	H265_NALU_TYPE_UNSPEC62 = 62,
	H265_NALU_TYPE_UNSPEC63 = 63,
};


/**
 * Table E.1
 */
enum h265_aspect_ratio {
	H265_ASPECT_RATIO_UNSPECIFIED = 0,
	H265_ASPECT_RATIO_1_1 = 1,
	H265_ASPECT_RATIO_12_11 = 2,
	H265_ASPECT_RATIO_10_11 = 3,
	H265_ASPECT_RATIO_16_11 = 4,
	H265_ASPECT_RATIO_40_33 = 5,
	H265_ASPECT_RATIO_24_11 = 6,
	H265_ASPECT_RATIO_20_11 = 7,
	H265_ASPECT_RATIO_32_11 = 8,
	H265_ASPECT_RATIO_80_33 = 9,
	H265_ASPECT_RATIO_18_11 = 10,
	H265_ASPECT_RATIO_15_11 = 11,
	H265_ASPECT_RATIO_64_33 = 12,
	H265_ASPECT_RATIO_160_99 = 13,
	H265_ASPECT_RATIO_4_3 = 14,
	H265_ASPECT_RATIO_3_2 = 15,
	H265_ASPECT_RATIO_2_1 = 16,
	/* 17 .. 254 reserved */
	H265_ASPECT_RATIO_EXTENDED_SAR = 255,
};


/**
 * 7.3.1 NAL unit syntax
 */
struct h265_nalu_header {
	uint32_t forbidden_zero_bit;
	uint32_t nal_unit_type;
	uint32_t nuh_layer_id;
	uint32_t nuh_temporal_id_plus1;
};


/**
 * 7.3.2.5 Access unit delimiter RBSP syntax
 */
struct h265_aud {
	uint32_t pic_type;
};


/**
 * Justification:
 * - 7.4.3.1: vps_max_sub_layers_minus1 in [0, 6]
 * - 7.4.3.2.1: sps_max_sub_layers_minus1 in [0, 6]
 */
#define SUB_LAYERS_MAX 7


/**
 * Fields common to general and sub-layer profile_tier_levels. This
 * factorization of a syntax structure is backed by the paragraph following the
 * definition of reserved_zero_2bits in 7.4.4
 */
struct h265_ptl_core {
	uint32_t profile_space;
	int tier_flag;
	uint32_t profile_idc;
	int profile_compatibility_flag[32];
	int progressive_source_flag;
	int interlaced_source_flag;
	int non_packed_constraint_flag;
	int frame_only_constraint_flag;

	int max_12bit_constraint_flag;
	int max_10bit_constraint_flag;
	int max_8bit_constraint_flag;
	int max_422chroma_constraint_flag;
	int max_420chroma_constraint_flag;
	int max_monochrome_constraint_flag;
	int intra_constraint_flag;
	int one_picture_only_constraint_flag;
	int lower_bit_rate_constraint_flag;
	int max_14bit_constraint_flag;

	int inbld_flag;

	uint32_t level_idc;
};


/**
 * 7.3.3 Profile, tier and level syntax
 */
struct h265_profile_tier_level {
	struct h265_ptl_core general;

	/* Size is maxNumSubLayersMinus1 */
	struct {
		int profile;
		int level;
	} sub_layer_present_flags[SUB_LAYERS_MAX];

	/* Size is maxNumSubLayersMinus1 */
	struct h265_ptl_core sub_layers[SUB_LAYERS_MAX];
};


/**
 * D.2 SEI payload syntax
 */
enum h265_sei_type {
	H265_SEI_TYPE_USER_DATA_UNREGISTERED = 5,
	H265_SEI_TYPE_RECOVERY_POINT = 6,
	H265_SEI_TYPE_TIME_CODE = 136,
	H265_SEI_TYPE_MASTERING_DISPLAY_COLOUR_VOLUME = 137,
	H265_SEI_TYPE_CONTENT_LIGHT_LEVEL = 144,
};


/**
 * D.2.7 User data unregistered SEI message syntax
 */
struct h265_sei_user_data_unregistered {
	uint8_t uuid[16];
	const uint8_t *buf;
	size_t len;
};


/**
 * D.2.8 Recovery point SEI message syntax
 */
struct h265_sei_recovery_point {
	int32_t recovery_poc_cnt;
	int exact_match_flag;
	int broken_link_flag;
};


/**
 * D.2.27 Time code SEI message syntax
 */
struct h265_sei_time_code {
	/* Range is 1-4 */
	uint8_t num_clock_ts;

	struct {
		int clock_timestamp_flag;
		int units_field_based_flag;
		uint8_t counting_type;
		int full_timestamp_flag;
		int discontinuity_flag;
		int cnt_dropped_flag;
		uint16_t n_frames;
		int seconds_flag;
		uint8_t seconds_value;
		int minutes_flag;
		uint8_t minutes_value;
		int hours_flag;
		uint8_t hours_value;
		uint8_t time_offset_length;
		int32_t time_offset_value;
	} clock_ts[4];
};


/**
 * D.2.28 Mastering display colour volume SEI message syntax
 */
struct h265_sei_mastering_display_colour_volume {
	uint16_t display_primaries_x[3];
	uint16_t display_primaries_y[3];
	uint16_t white_point_x;
	uint16_t white_point_y;
	uint32_t max_display_mastering_luminance;
	uint32_t min_display_mastering_luminance;
};


/**
 * D.2.35 Content light level information SEI message syntax
 */
struct h265_sei_content_light_level {
	uint16_t max_content_light_level;
	uint16_t max_pic_average_light_level;
};


struct h265_sei {
	enum h265_sei_type type;

	union {
		struct h265_sei_user_data_unregistered user_data_unregistered;
		struct h265_sei_recovery_point recovery_point;
		struct h265_sei_time_code time_code;
		struct h265_sei_mastering_display_colour_volume
			mastering_display_colour_volume;
		struct h265_sei_content_light_level content_light_level;
	};

	/* Internal use only */
	struct {
		uint8_t *buf;
		size_t len;
	} raw;
};


/**
 * Justification:
 * - E.3.3:
 *     > The variable CpbCnt is set equal to cpb_cnt_minus1[subLayerId] + 1
 * - E.3.2: cpb_cnt_minus1 in [0, 31]
 */
#define CPBS_MAX 32


/**
 * E.2.3 Sub-layer HRD parameters syntax
 */
struct h265_sub_layer_hrd {
	struct {
		uint32_t bit_rate_value_minus1;
		uint32_t size_value_minus1;
		uint32_t size_du_value_minus1;
		uint32_t bit_rate_du_value_minus1;
		int flag;
	} cpbs[CPBS_MAX];
};


/**
 * E.2.2 HRD parameters syntax
 */
struct h265_hrd {
	int nal_hrd_parameters_present_flag;
	int vcl_hrd_parameters_present_flag;

	int sub_pic_hrd_params_present_flag;

	uint32_t tick_divisor_minus2;
	uint32_t du_cpb_removal_delay_increment_length_minus1;
	int sub_pic_cpb_params_in_pic_timing_sei_flag;
	uint32_t dpb_output_delay_du_length_minus1;

	uint32_t bit_rate_scale;
	uint32_t cpb_size_scale;

	uint32_t cpb_size_du_scale;

	uint32_t initial_cpb_removal_delay_length_minus1;
	uint32_t au_cpb_removal_delay_length_minus1;
	uint32_t dpb_output_delay_length_minus1;

	struct {
		int fixed_pic_rate_general_flag;
		int fixed_pic_rate_within_cvs_flag;
		uint32_t elemental_duration_in_tc_minus1;
		int low_delay_hrd_flag;

		uint32_t cpb_cnt_minus1;
		struct h265_sub_layer_hrd nal_hrd;
		struct h265_sub_layer_hrd vcl_hrd;
	} sub_layers[SUB_LAYERS_MAX];
};

/**
 * Justification:
 * - 7.4.3.1: vps_max_layers_minus1; 64 is a corner case but should be allowed
 */
#define LAYERS_MAX 64


/**
 * Justification:
 * - 7.4.3.1: vps_num_layer_sets_minus1 in [0, 1023]
 */
#define LAYER_SETS_MAX 1024


/**
 * 7.3.2.1 Video parameter set RBSP syntax
 */
struct h265_vps {
	uint32_t vps_video_parameter_set_id;
	int vps_base_layer_internal_flag;
	int vps_base_layer_available_flag;
	uint32_t vps_max_layers_minus1;

	/* Range is 0-6 */
	uint32_t vps_max_sub_layers_minus1;

	int vps_temporal_id_nesting_flag;
	uint32_t vps_reserved_0xffff_16bits;

	struct h265_profile_tier_level profile_tier_level;

	int vps_sub_layer_ordering_info_present_flag;

	uint32_t vps_max_dec_pic_buffering_minus1[SUB_LAYERS_MAX];
	uint32_t vps_max_num_reorder_pics[SUB_LAYERS_MAX];
	uint32_t vps_max_latency_increase_plus1[SUB_LAYERS_MAX];

	/* Range is 0-63, with 63 being a corner case */
	uint32_t vps_max_layer_id;

	/* Range is 0-1023 */
	uint32_t vps_num_layer_sets_minus1;

	int layer_id_included_flag[LAYER_SETS_MAX][LAYERS_MAX];

	int vps_timing_info_present_flag;
	uint32_t vps_num_units_in_tick;
	uint32_t vps_time_scale;
	int vps_poc_proportional_to_timing_flag;
	uint32_t vps_num_ticks_poc_diff_one_minus1;

	/* Range is vps_num_layer_sets_minus1 + 1 */
	uint32_t vps_num_hrd_parameters;

	uint32_t hrd_layer_set_idx[LAYER_SETS_MAX];
	int cprms_present_flag[LAYER_SETS_MAX];
	struct h265_hrd hrd_parameters[LAYER_SETS_MAX];

	int vps_extension_flag;
};


/**
 * 7.3.4 Scaling list syntax
 */
struct h265_scaling_list_data {
	int pred_mode_flag[4][6];
	uint32_t pred_matrix_id_delta[4][6];
	int32_t dc_coef_minus8[4][6];
	int32_t delta_coef[4][6][64];
};


/**
 * 7.3.7 Short-term reference picture set syntax
 */
struct h265_st_ref_pic_set {
	int inter_ref_pic_set_prediction_flag;
	uint32_t delta_idx_minus1;
	int delta_rps_sign;
	uint32_t abs_delta_rps_minus1;

	/**
	 * The maximum size of those is obtained by considering the following:
	 *   - Equation (7-71): NumDeltaPocs = NumNegativePics + NumPositivePics
	 * in section 7.4.8
	 *   - Semantics of num_negative_pics and num_positive_pics on page 104
	 *   - Semantics of sps_max_dec_pic_buffering_minus1 on page 79
	 *   - Derivation of MaxDpbSize on page 263 (Annex A);
	 */
	int used_by_curr_pic_flag[16];
	int use_delta_flag[16];

	uint32_t num_negative_pics;
	uint32_t num_positive_pics;
	uint32_t delta_poc_s0_minus1[16];
	int used_by_curr_pic_s0_flag[16];
	uint32_t delta_poc_s1_minus1[16];
	int used_by_curr_pic_s1_flag[16];

	/**
	 * Those are not syntax elements but derived values useful for decoding
	 * the subsequent rp sets. Be wary that the derivation process for them
	 * is not the expected `derived_delta_poc_s0[i] =
	 * derived_delta_poc_s0_minus1[i] + 1`. Derivation is explained
	 * in 7.4.8;
	 */
	int32_t derived_delta_poc_s0[16];
	int32_t derived_delta_poc_s1[16];
};


/**
 * 7.3.2.2.2 Sequence parameter set range extension syntax
 */
struct h265_sps_range_ext {
	int transform_skip_rotation_enabled_flag;
	int transform_skip_context_enabled_flag;
	int implicit_rdpcm_enabled_flag;
	int explicit_rdpcm_enabled_flag;
	int extended_precision_processing_flag;
	int intra_smoothing_disabled_flag;
	int high_precision_offsets_enabled_flag;
	int persistent_rice_adaptation_enabled_flag;
	int cabac_bypass_alignment_enabled_flag;
};


/**
 * F.7.3.2.2.4 Sequence parameter set multilayer extension syntax
 */
struct h265_sps_multilayer_ext {
	int inter_view_mv_vert_constraint_flag;
};


/**
 * I.7.3.2.2.5 Sequence parameter set 3D extension syntax
 */
struct h265_sps_3d_ext {
	int iv_di_mc_enabled_flag[2];
	int iv_mv_scal_enabled_flag[2];

	uint32_t log2_ivmc_sub_pb_size_minus3[2];
	int iv_res_pred_enabled_flag[2];
	int depth_ref_enabled_flag[2];
	int vsp_mc_enabled_flag[2];
	int dbbp_enabled_flag[2];

	int tex_mc_enabled_flag[2];
	uint32_t log2_texmc_sub_pb_size_minus3[2];
	int intra_contour_enabled_flag[2];
	int intra_dc_only_wedge_enabled_flag[2];
	int cqt_cu_part_pred_enabled_flag[2];
	int inter_dc_only_enabled_flag[2];
	int skip_intra_enabled_flag[2];
};


/**
 * 7.3.2.2.3 Sequence parameter set screen content coding extension syntax
 */
struct h265_sps_scc_ext {
	int sps_curr_pic_ref_enabled_flag;
	int palette_mode_enabled_flag;
	uint32_t palette_max_size;
	uint32_t delta_palette_max_predictor_size;
	int sps_palette_predictor_initializer_present_flag;

	/**
	 * Range is 0..(PaletteMaxPredictorSize-1)
	 * PaletteMaxPredictorSize =
	 * palette_max_size + delta_palette_max_predictor_size
	 */
	uint32_t sps_num_palette_predictor_initializer_minus1;

	/**
	 * numComps * PaletteMaxPredictorSize elements
	 * numComps = (chroma_format_idc == 0) ? 1 : 3
	 *
	 * For the 128 upper size limit, see Annex A page 258
	 */
	uint32_t sps_palette_predictor_initializers[3][128];

	uint32_t motion_vector_resolution_control_idc;
	int intra_boundary_filtering_disabled_flag;
};


/**
 * E.2.1 VUI parameters syntax.
 */
struct h265_vui {
	int aspect_ratio_info_present_flag;
	uint32_t aspect_ratio_idc;
	uint32_t sar_width;
	uint32_t sar_height;

	int overscan_info_present_flag;
	int overscan_appropriate_flag;

	int video_signal_type_present_flag;
	uint32_t video_format;
	int video_full_range_flag;
	int colour_description_present_flag;
	uint32_t colour_primaries;
	uint32_t transfer_characteristics;
	uint32_t matrix_coeffs;

	int chroma_loc_info_present_flag;
	uint32_t chroma_sample_loc_type_top_field;
	uint32_t chroma_sample_loc_type_bottom_field;

	int neutral_chroma_indication_flag;
	int field_seq_flag;
	int frame_field_info_present_flag;

	int default_display_window_flag;
	uint32_t def_disp_win_left_offset;
	uint32_t def_disp_win_right_offset;
	uint32_t def_disp_win_top_offset;
	uint32_t def_disp_win_bottom_offset;

	int vui_timing_info_present_flag;
	uint32_t vui_num_units_in_tick;
	uint32_t vui_time_scale;
	int vui_poc_proportional_to_timing_flag;
	uint32_t vui_num_ticks_poc_diff_one_minus1;
	int vui_hrd_parameters_present_flag;
	struct h265_hrd hrd;

	int bitstream_restriction_flag;
	int tiles_fixed_structure_flag;
	int motion_vectors_over_pic_boundaries_flag;
	int restricted_ref_pic_lists_flag;
	uint32_t min_spatial_segmentation_idc;
	uint32_t max_bytes_per_pic_denom;
	uint32_t max_bits_per_min_cu_denom;
	uint32_t log2_max_mv_length_horizontal;
	uint32_t log2_max_mv_length_vertical;
};


/**
 * 7.3.2.2 Sequence parameter set RBSP syntax
 */
struct h265_sps {
	uint32_t sps_video_parameter_set_id;

	/* Range is 0-6 */
	uint32_t sps_max_sub_layers_minus1;

	int sps_temporal_id_nesting_flag;

	struct h265_profile_tier_level profile_tier_level;

	uint32_t sps_seq_parameter_set_id;
	uint32_t chroma_format_idc;
	int separate_colour_plane_flag;
	uint32_t pic_width_in_luma_samples;
	uint32_t pic_height_in_luma_samples;
	int conformance_window_flag;
	uint32_t conf_win_left_offset;
	uint32_t conf_win_right_offset;
	uint32_t conf_win_top_offset;
	uint32_t conf_win_bottom_offset;
	uint32_t bit_depth_luma_minus8;
	uint32_t bit_depth_chroma_minus8;
	uint32_t log2_max_pic_order_cnt_lsb_minus4;
	int sps_sub_layer_ordering_info_present_flag;

	uint32_t sps_max_dec_pic_buffering_minus1[SUB_LAYERS_MAX];
	uint32_t sps_max_num_reorder_pics[SUB_LAYERS_MAX];
	uint32_t sps_max_latency_increase_plus1[SUB_LAYERS_MAX];

	uint32_t log2_min_luma_coding_block_size_minus3;
	uint32_t log2_diff_max_min_luma_coding_block_size;
	uint32_t log2_min_luma_transform_block_size_minus2;
	uint32_t log2_diff_max_min_luma_transform_block_size;
	uint32_t max_transform_hierarchy_depth_inter;
	uint32_t max_transform_hierarchy_depth_intra;
	int scaling_list_enabled_flag;
	int sps_scaling_list_data_present_flag;

	struct h265_scaling_list_data scaling_list_data;

	int amp_enabled_flag;
	int sample_adaptive_offset_enabled_flag;
	int pcm_enabled_flag;
	uint32_t pcm_sample_bit_depth_luma_minus1;
	uint32_t pcm_sample_bit_depth_chroma_minus1;
	uint32_t log2_min_pcm_luma_coding_block_size_minus3;
	uint32_t log2_diff_max_min_pcm_luma_coding_block_size;
	int pcm_loop_filter_disabled_flag;

	/* Range is 0-64 */
	uint32_t num_short_term_ref_pic_sets;
	struct h265_st_ref_pic_set st_ref_pic_sets[64];

	int long_term_ref_pics_present_flag;

	/* Range is 0-32 */
	uint32_t num_long_term_ref_pics_sps;

	uint32_t lt_ref_pic_poc_lsb_sps[32];
	int used_by_curr_pic_lt_sps_flag[32];

	int sps_temporal_mvp_enabled_flag;
	int strong_intra_smoothing_enabled_flag;
	int vui_parameters_present_flag;

	struct h265_vui vui;

	int sps_extension_present_flag;
	int sps_range_extension_flag;
	int sps_multilayer_extension_flag;
	int sps_3d_extension_flag;
	int sps_scc_extension_flag;
	uint32_t sps_extension_4bits;

	struct h265_sps_range_ext sps_range_ext;

	struct h265_sps_multilayer_ext sps_multilayer_ext;

	struct h265_sps_3d_ext sps_3d_ext;

	struct h265_sps_scc_ext sps_scc_ext;
};


/**
 * 7.3.2.3.2 Picture parameter set range extension syntax
 */
struct h265_pps_range_ext {
	uint32_t log2_max_transform_skip_block_size_minus2;
	int cross_component_prediction_enabled_flag;
	int chroma_qp_offset_list_enabled_flag;
	uint32_t diff_cu_chroma_qp_offset_depth;

	/* Range is 0-5 */
	uint32_t chroma_qp_offset_list_len_minus1;

	int32_t cb_qp_offset_list[6];
	int32_t cr_qp_offset_list[6];

	uint32_t log2_sao_offset_scale_luma;
	uint32_t log2_sao_offset_scale_chroma;
};


/**
 * I.7.3.2.3.8 Delta depth look-up table syntax
 */
struct h265_delta_dlt {
	uint32_t num_val_delta_dlt;

	uint32_t max_diff;
	uint32_t min_diff_minus1;

	uint32_t delta_dlt_val0;

	uint32_t *delta_val_diff_minus_min;
};


/**
 * I.7.3.2.3.7 Picture parameter set 3D extension syntax
 */
struct h265_pps_3d_ext {
	int dlts_present_flag;
	uint32_t pps_depth_layers_minus1;
	uint32_t pps_bit_depth_for_depth_layers_minus8;

	int dlt_flag[LAYERS_MAX];
	int dlt_pred_flag[LAYERS_MAX];
	int dlt_val_flags_present_flag[LAYERS_MAX];

	int *dlt_value_flag[LAYERS_MAX];

	struct h265_delta_dlt delta_dlt[LAYERS_MAX];
};


/**
 * 7.3.2.3.3 Picture parameter set screen content coding extension syntax
 */
struct h265_pps_scc_ext {
	int pps_curr_pic_ref_enabled_flag;
	int residual_adaptive_colour_transform_enabled_flag;
	int pps_slice_act_qp_offsets_present_flag;
	int32_t pps_act_y_qp_offset_plus5;
	int32_t pps_act_cb_qp_offset_plus5;
	int32_t pps_act_cr_qp_offset_plus3;
	int pps_palette_predictor_initializers_present_flag;

	/**
	 * Range is 0..(PaletteMaxPredictorSize-1)
	 * PaletteMaxPredictorSize =
	 * palette_max_size + delta_palette_max_predictor_size
	 */
	uint32_t pps_num_palette_predictor_initializers;

	int monochrome_palette_flag;
	uint32_t luma_bit_depth_entry_minus8;
	uint32_t chroma_bit_depth_entry_minus8;

	/**
	 * numComps * pps_num_palette_predictor_initializer elements
	 * numComps = monochrome_palette_flag ? 1 : 3
	 *
	 * 7.4.3.3.3
	 * > pps_num_palette_predictor_initializers shall be less than
	 * PaletteMaxPredictorSize
	 *
	 * A.3.7:
	 * > PaletteMaxPredictorSize shall be less than or equal to 128
	 */
	uint32_t pps_palette_predictor_initializer[3][128];
};


/**
 * 7.3.2.3.1 General picture parameter set RBSP syntax
 */
struct h265_pps {
	uint32_t pps_pic_parameter_set_id;
	uint32_t pps_seq_parameter_set_id;
	int dependent_slice_segments_enabled_flag;
	int output_flag_present_flag;
	uint32_t num_extra_slice_header_bits;
	int sign_data_hiding_enabled_flag;
	int cabac_init_present_flag;
	uint32_t num_ref_idx_l0_default_active_minus1;
	uint32_t num_ref_idx_l1_default_active_minus1;
	int32_t init_qp_minus26;
	int constrained_intra_pred_flag;
	int transform_skip_enabled_flag;
	int cu_qp_delta_enabled_flag;
	uint32_t diff_cu_qp_delta_depth;
	int32_t pps_cb_qp_offset;
	int32_t pps_cr_qp_offset;
	int pps_slice_chroma_qp_offsets_present_flag;
	int weighted_pred_flag;
	int weighted_bipred_flag;
	int transquant_bypass_enabled_flag;
	int tiles_enabled_flag;
	int entropy_coding_sync_enabled_flag;
	uint32_t num_tile_columns_minus1;
	uint32_t num_tile_rows_minus1;
	int uniform_spacing_flag;

	int loop_filter_across_tiles_enabled_flag;
	int pps_loop_filter_across_slices_enabled_flag;
	int deblocking_filter_control_present_flag;
	int deblocking_filter_override_enabled_flag;
	int pps_deblocking_filter_disabled_flag;
	int32_t pps_beta_offset_div2;
	int32_t pps_tc_offset_div2;
	int pps_scaling_list_data_present_flag;

	struct h265_scaling_list_data scaling_list_data;

	int lists_modification_present_flag;
	uint32_t log2_parallel_merge_level_minus2;
	int slice_segment_header_extension_present_flag;
	int pps_extension_present_flag;
	int pps_range_extension_flag;
	int pps_multilayer_extension_flag;
	int pps_3d_extension_flag;
	int pps_scc_extension_flag;
	uint32_t pps_extension_4bits;

	struct h265_pps_range_ext pps_range_ext;

	/* TODO: pps_multilayer_extension() */

	struct h265_pps_3d_ext pps_3d_ext;

	struct h265_pps_scc_ext pps_scc_ext;

	/* these two dynamic arrays must be declared
	 * at the end of the PPS struct
	 * to allow deep compare with memcmp */

	/* num_tile_columns_minus1 elements */
	uint32_t *column_width_minus1;

	/* num_tile_rows_minus1 elements */
	uint32_t *row_height_minus1;
};


/* Extra info from parameter sets */
struct h265_info {
	/* Picture width in pixels */
	uint32_t width;

	/* Picture height in pixels */
	uint32_t height;

	/* Luma bit depth */
	uint8_t bit_depth_luma;

	/* Source aspect ratio width (1 if unknown) */
	uint32_t sar_width;

	/* Source aspect ratio width (1 if unknown) */
	uint32_t sar_height;

	/* Left crop in pixels */
	uint32_t crop_left;

	/* Top crop in pixels */
	uint32_t crop_top;

	/* Crop width in pixels (equal to picture width if no crop) */
	uint32_t crop_width;

	/* Crop height in pixels (equal to picture height if no crop) */
	uint32_t crop_height;

	/* Full range flag */
	int full_range;

	/* 1: colour_primaries, transfer_characteristics and
	 * matrix_coefficients are valid; 0 otherwise */
	int colour_description_present;

	/* Colour primaries */
	uint32_t colour_primaries;

	/* Transfer characteristics */
	uint32_t transfer_characteristics;

	/* Matrix coefficients */
	uint32_t matrix_coefficients;

	/* Number of time units of a clock tick (0 if unknown) */
	uint32_t num_units_in_tick;

	/* Number of time units in one second (0 if unknown) */
	uint32_t time_scale;

	/* Declared framerate from time_scale and num_units_in_tick
	 * (0 if unknown) */
	float framerate;

	/* Declared framerate from time_scale and num_units_in_tick
	 * (0 if unknown) - numerator */
	uint32_t framerate_num;

	/* Declared framerate from time_scale and num_units_in_tick
	 * (0 if unknown) - denominator */
	uint32_t framerate_den;

	/* NAL HRD bitrate (0 if unknown) */
	uint32_t nal_hrd_bitrate;

	/* NAL HRD CPB size (0 if unknown) */
	uint32_t nal_hrd_cpb_size;

	/* VCL HRD bitrate (0 if unknown) */
	uint32_t vcl_hrd_bitrate;

	/* VCL HRD CPB size (0 if unknown) */
	uint32_t vcl_hrd_cpb_size;
};


H265_API
int h265_delta_dlt_clear(struct h265_delta_dlt *dlt);


H265_API
int h265_pps_3d_ext_clear(struct h265_pps_3d_ext *ext);


H265_API
int h265_pps_clear(struct h265_pps *pps);


H265_API
int h265_pps_cpy(struct h265_pps *dst_pps, const struct h265_pps *src_pps);


H265_API
int h265_pps_cmp(const struct h265_pps *pps1, const struct h265_pps *pps2);


H265_API
const char *h265_nalu_type_str(enum h265_nalu_type val);


H265_API
const char *h265_sei_type_str(enum h265_sei_type val);


#endif /* !_H265_TYPES_H_ */
