/*
 * This file is part of MPlayer CE.
 *
 * MPlayer CE is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * MPlayer CE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with MPlayer CE; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavcodec/dsputil.h"
#include "libavcodec/h264data.h"
#include "libavcodec/h264dsp.h"
#include "dsputil_paired.h"
#include "libavutil/ppc/paired.h"

static void ff_h264_idct_add_paired(uint8_t *dst, DCTELEM *block, int stride)
{
	const vector float half = {0.5,0.5};
	const float scalar = 0.015625;
	vector float pair[2], sub[2], add[2];
	vector float result, element[8];
	
	block[0] += 32;
	uint8_t *base[2] = {dst,dst};
	
	pair[0] = psq_l(0,block,0,7);
	pair[1] = psq_l(16,block,0,7);
	add[0] = paired_add(pair[0], pair[1]);
	sub[0] = paired_sub(pair[0], pair[1]);
	
	pair[0] = psq_l(8,block,0,7);
	pair[1] = psq_l(24,block,0,7);
	sub[1] = paired_msub(pair[0], half, pair[1]);
	add[1] = paired_madd(pair[1], half, pair[0]);
	
	pair[0] = paired_add(add[0], add[1]);
	pair[1] = paired_add(sub[0], sub[1]);
	
	element[0] = paired_merge00(pair[0], pair[1]);
	element[2] = paired_merge11(pair[0], pair[1]);
	
	pair[0] = paired_sub(sub[0], sub[1]);
	pair[1] = paired_sub(add[0], add[1]);
	
	element[1] = paired_merge00(pair[0], pair[1]);
	element[3] = paired_merge11(pair[0], pair[1]);
	
	pair[0] = psq_l(4,block,0,7);
	pair[1] = psq_l(20,block,0,7);
	add[0] = paired_add(pair[0], pair[1]);
	sub[0] = paired_sub(pair[0], pair[1]);
	
	pair[0] = psq_l(12,block,0,7);
	pair[1] = psq_l(28,block,0,7);
	sub[1] = paired_msub(pair[0], half, pair[1]);
	add[1] = paired_madd(pair[1], half, pair[0]);
	
	pair[0] = paired_add(add[0], add[1]);
	pair[1] = paired_add(sub[0], sub[1]);
	
	element[4] = paired_merge00(pair[0], pair[1]);
	element[6] = paired_merge11(pair[0], pair[1]);
	
	pair[0] = paired_sub(sub[0], sub[1]);
	pair[1] = paired_sub(add[0], add[1]);
	
	element[5] = paired_merge00(pair[0], pair[1]);
	element[7] = paired_merge11(pair[0], pair[1]);
	
	add[0] = paired_add(element[0], element[4]);
	sub[0] = paired_sub(element[0], element[4]);
	
	sub[1] = paired_msub(element[2], half, element[6]);
	add[1] = paired_madd(element[6], half, element[2]);
	
	result = psq_l(0,base[0],0,4);
	result = ps_madds0(paired_add(add[0], add[1]), scalar, result);
	psq_st(result,0,base[0],0,4);
	
	result = psq_lux(base[0],stride,0,4);
	result = ps_madds0(paired_add(sub[0], sub[1]), scalar, result);
	psq_st(result,0,base[0],0,4);
	
	result = psq_lux(base[0],stride,0,4);
	result = ps_madds0(paired_sub(sub[0], sub[1]), scalar, result);
	psq_st(result,0,base[0],0,4);
	
	result = psq_lux(base[0],stride,0,4);
	result = ps_madds0(paired_sub(add[0], add[1]), scalar, result);
	psq_stx(result,0,base[0],0,4);
	
	add[0] = paired_add(element[1], element[5]);
	sub[0] = paired_sub(element[1], element[5]);
	
	sub[1] = paired_msub(element[3], half, element[7]);
	add[1] = paired_madd(element[7], half, element[3]);
	
	result = psq_lu(2,base[1],0,4);
	result = ps_madds0(paired_add(add[0], add[1]), scalar, result);
	psq_st(result,0,base[1],0,4);
	
	result = psq_lux(base[1],stride,0,4);
	result = ps_madds0(paired_add(sub[0], sub[1]), scalar, result);
	psq_st(result,0,base[1],0,4);
	
	result = psq_lux(base[1],stride,0,4);
	result = ps_madds0(paired_sub(sub[0], sub[1]), scalar, result);
	psq_st(result,0,base[1],0,4);
	
	result = psq_lux(base[1],stride,0,4);
	result = ps_madds0(paired_sub(add[0], add[1]), scalar, result);
	psq_st(result,0,base[1],0,4);
}

static void ff_h264_idct_dc_add_paired(uint8_t *dst, DCTELEM *block, int stride)
{
	const vector float half = {0.5,0.5};
	const float scalar = 0.015625;
	vector float pair, offset;
	
	offset = psq_l(0,block,1,7);
	offset = paired_merge00(offset, offset);
	offset = ps_madds0(offset, scalar, half);
	
	dst -= stride;
	
	for (int i=0; i<4; i++) {
		pair = psq_lux(dst,stride,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,0,dst,0,4);
		
		pair = psq_l(2,dst,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,2,dst,0,4);
	}
}

static void ff_h264_idct_add16_paired(uint8_t *dst, const int *block_offset, DCTELEM *block, int stride, const uint8_t nnzc[48])
{
	for (int i=0; i<16; i++) {
		int nnz = nnzc[scan8[i]];
		if(nnz) {
			if (nnz==1 && block[i*16])
				ff_h264_idct_dc_add_paired(dst + block_offset[i], block + i*16, stride);
			else
				ff_h264_idct_add_paired(dst + block_offset[i], block + i*16, stride);
		}
	}
}

static void ff_h264_idct_add16intra_paired(uint8_t *dst, const int *block_offset, DCTELEM *block, int stride, const uint8_t nnzc[48])
{
	for (int i=0; i<16; i++) {
		if (nnzc[scan8[i]])
			ff_h264_idct_add_paired(dst + block_offset[i], block + i*16, stride);
		else if (block[i*16])
			ff_h264_idct_dc_add_paired(dst + block_offset[i], block + i*16, stride);
	}
}

static void ff_h264_idct_add8_paired(uint8_t **dest, const int *block_offset, DCTELEM *block, int stride, const uint8_t nnzc[48])
{
	for (int i=16; i<16+8; i++) {
		if (nnzc[scan8[i]])
			ff_h264_idct_add_paired(dest[(i&4)>>2] + block_offset[i], block + i*16, stride);
		else if (block[i*16])
			ff_h264_idct_dc_add_paired(dest[(i&4)>>2] + block_offset[i], block + i*16, stride);
	}
}

// FIXME: Rounding errors.
static void ff_h264_idct8_add_paired(uint8_t *dst, DCTELEM *block, int stride)
{
	const vector float half = {0.5,0.5};
	const vector float quarter = {0.25,0.25};
	const float scalar = 0.015625;
	vector float pair[4], sub[4], add[4];
	vector float result, pre[4];
	
	block[0] += 32;
	block -= 2;
	
	int16_t element[64];
	int16_t *base[2] = {element-16,element-2};
	
	int i;
	for (i=0; i<8; i+=2) {
		pair[0] = psq_lu(4,block,0,7);
		pair[1] = psq_l(64,block,0,7);
		pre[0] = paired_add(pair[0], pair[1]);
		pre[1] = paired_sub(pair[0], pair[1]);
		
		pair[0] = psq_l(32,block,0,7);
		pair[1] = psq_l(96,block,0,7);
		pre[2] = paired_msub(pair[0], half, pair[1]);
		pre[3] = paired_madd(pair[1], half, pair[0]);
		
		add[0] = paired_add(pre[0], pre[3]);
		add[1] = paired_add(pre[1], pre[2]);
		sub[0] = paired_sub(pre[1], pre[2]);
		sub[1] = paired_sub(pre[0], pre[3]);
		
		pair[0] = psq_l(16,block,0,7);
		pair[1] = psq_l(48,block,0,7);
		pair[2] = psq_l(80,block,0,7);
		pair[3] = psq_l(112,block,0,7);
		
		pre[0] = paired_sub(paired_sub(pair[2], pair[1]), paired_madd(pair[3], half, pair[3]));
		pre[1] = paired_sub(paired_add(pair[0], pair[3]), paired_madd(pair[1], half, pair[1]));
		pre[2] = paired_add(paired_sub(pair[3], pair[0]), paired_madd(pair[2], half, pair[2]));
		pre[3] = paired_add(paired_add(pair[1], pair[2]), paired_madd(pair[0], half, pair[0]));
		
		add[2] = paired_madd(pre[3], quarter, pre[0]);
		add[3] = paired_madd(pre[2], quarter, pre[1]);
		sub[2] = paired_msub(pre[1], quarter, pre[2]);
		sub[3] = paired_sub(pre[3], paired_mul(pre[0], quarter));
		
		pair[0] = paired_add(add[0], sub[3]);
		pair[1] = paired_add(add[1], sub[2]);
		
		psq_stu(paired_merge00(pair[0], pair[1]),32,base[0],0,7);
		psq_st(paired_merge11(pair[0], pair[1]),16,base[0],0,7);
		
		pair[0] = paired_add(sub[0], add[3]);
		pair[1] = paired_add(sub[1], add[2]);
		
		psq_st(paired_merge00(pair[0], pair[1]),4,base[0],0,7);
		psq_st(paired_merge11(pair[0], pair[1]),20,base[0],0,7);
		
		pair[0] = paired_sub(sub[1], add[2]);
		pair[1] = paired_sub(sub[0], add[3]);
		
		psq_st(paired_merge00(pair[0], pair[1]),8,base[0],0,7);
		psq_st(paired_merge11(pair[0], pair[1]),24,base[0],0,7);
		
		pair[0] = paired_sub(add[1], sub[2]);
		pair[1] = paired_sub(add[0], sub[3]);
		
		psq_st(paired_merge00(pair[0], pair[1]),12,base[0],0,7);
		psq_st(paired_merge11(pair[0], pair[1]),28,base[0],0,7);
	}
	
	uint8_t *in_col = dst-2;
	
	for (i=0; i<8; i+=2) {
		pair[0] = psq_lu(4,base[1],0,7);
		pair[1] = psq_l(64,base[1],0,7);
		pre[0] = paired_add(pair[0], pair[1]);
		pre[1] = paired_sub(pair[0], pair[1]);
		
		pair[0] = psq_l(32,base[1],0,7);
		pair[1] = psq_l(96,base[1],0,7);
		pre[2] = paired_msub(pair[0], half, pair[1]);
		pre[3] = paired_madd(pair[1], half, pair[0]);
		
		add[0] = paired_add(pre[0], pre[3]);
		add[1] = paired_add(pre[1], pre[2]);
		sub[0] = paired_sub(pre[1], pre[2]);
		sub[1] = paired_sub(pre[0], pre[3]);
		
		pair[0] = psq_l(16,base[1],0,7);
		pair[1] = psq_l(48,base[1],0,7);
		pair[2] = psq_l(80,base[1],0,7);
		pair[3] = psq_l(112,base[1],0,7);
		
		pre[0] = paired_sub(paired_sub(pair[2], pair[1]), paired_madd(pair[3], half, pair[3]));
		pre[1] = paired_sub(paired_add(pair[0], pair[3]), paired_madd(pair[1], half, pair[1]));
		pre[2] = paired_add(paired_sub(pair[3], pair[0]), paired_madd(pair[2], half, pair[2]));
		pre[3] = paired_add(paired_add(pair[1], pair[2]), paired_madd(pair[0], half, pair[0]));
		
		add[2] = paired_madd(pre[3], quarter, pre[0]);
		add[3] = paired_madd(pre[2], quarter, pre[1]);
		sub[2] = paired_msub(pre[1], quarter, pre[2]);
		sub[3] = paired_sub(pre[3], paired_mul(pre[0], quarter));
		
		result = psq_lu(2,in_col,0,4);
		result = ps_madds0(paired_add(add[0], sub[3]), scalar, result);
		psq_st(result,0,in_col,0,4);
		uint8_t *in_row = in_col;
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madds0(paired_add(add[1], sub[2]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madds0(paired_add(sub[0], add[3]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madds0(paired_add(sub[1], add[2]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madds0(paired_sub(sub[1], add[2]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madds0(paired_sub(sub[0], add[3]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madds0(paired_sub(add[1], sub[2]), scalar, result);
		psq_st(result,0,in_row,0,4);
		
		result = psq_lux(in_row,stride,0,4);
		result = ps_madds0(paired_sub(add[0], sub[3]), scalar, result);
		psq_st(result,0,in_row,0,4);
	}
}

static void ff_h264_idct8_dc_add_paired(uint8_t *dst, DCTELEM *block, int stride)
{
	const vector float half = {0.5,0.5};
	const float scalar = 0.015625;
	vector float pair, offset;
	
	offset = psq_l(0,block,1,7);
	offset = paired_merge00(offset, offset);
	offset = ps_madds0(offset, scalar, half);
	
	dst -= stride;
	
	for (int i=0; i<8; i++) {
		pair = psq_lux(dst,stride,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,0,dst,0,4);
		
		pair = psq_l(2,dst,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,2,dst,0,4);
		
		pair = psq_l(4,dst,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,4,dst,0,4);
		
		pair = psq_l(6,dst,0,4);
		pair = paired_add(pair, offset);
		psq_st(pair,6,dst,0,4);
	}
}

static void ff_h264_idct8_add4_paired(uint8_t *dst, const int *block_offset, DCTELEM *block, int stride, const uint8_t nnzc[48])
{
	for (int i=0; i<16; i+=4) {
		int nnz = nnzc[scan8[i]];
		if (nnz) {
			if (nnz==1 && block[i*16])
				ff_h264_idct8_dc_add_paired(dst + block_offset[i], block + i*16, stride);
			else
				ff_h264_idct8_add_paired(dst + block_offset[i], block + i*16, stride);
		}
	}
}

void ff_h264dsp_init_ppc(H264DSPContext *c)
{
	c->h264_idct_add = ff_h264_idct_add_paired;
	c->h264_idct_add8 = ff_h264_idct_add8_paired;
	c->h264_idct_add16 = ff_h264_idct_add16_paired;
	c->h264_idct_add16intra = ff_h264_idct_add16intra_paired;
	c->h264_idct_dc_add = ff_h264_idct_dc_add_paired;
	c->h264_idct8_dc_add = ff_h264_idct8_dc_add_paired;
	c->h264_idct8_add = ff_h264_idct8_add_paired;
	c->h264_idct8_add4 = ff_h264_idct8_add4_paired;
}
