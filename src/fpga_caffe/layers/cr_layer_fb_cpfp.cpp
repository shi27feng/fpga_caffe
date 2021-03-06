#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "../../../include/fpga_caffe/layer.hpp"
#include "../../../include/fpga_caffe/cpfp.hpp"
#include "../../../include/fpga_caffe/vector_types.hpp"

#define OCFACT 1 

void relu_fw(cpfp16 outbuf[OCFACT][512], char16 outbuf_relu[OCFACT][512],
  int k, int num_iter) {
  RELU_FW: for (int i = 0; i < num_iter; ++i) {
  #pragma HLS pipeline
    cpfp16 val;
    val.s0 = max(outbuf[k][i].s0);
    val.s1 = max(outbuf[k][i].s1);
    val.s2 = max(outbuf[k][i].s2);
    val.s3 = max(outbuf[k][i].s3);
    val.s4 = max(outbuf[k][i].s4);
    val.s5 = max(outbuf[k][i].s5);
    val.s6 = max(outbuf[k][i].s6);
    val.s7 = max(outbuf[k][i].s7);
    val.s8 = max(outbuf[k][i].s8);
    val.s9 = max(outbuf[k][i].s9);
    val.sa = max(outbuf[k][i].sa);
    val.sb = max(outbuf[k][i].sb);
    val.sc = max(outbuf[k][i].sc);
    val.sd = max(outbuf[k][i].sd);
    val.se = max(outbuf[k][i].se);
    val.sf = max(outbuf[k][i].sf);

    outbuf[k][i] = val;

    outbuf_relu[k][i].s0 = (val.s0 != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].s1 = (val.s1 != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].s2 = (val.s2 != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].s3 = (val.s3 != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].s4 = (val.s4 != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].s5 = (val.s5 != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].s6 = (val.s6 != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].s7 = (val.s7 != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].s8 = (val.s8 != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].s9 = (val.s9 != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].sa = (val.sa != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].sb = (val.sb != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].sc = (val.sc != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].sd = (val.sd != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].se = (val.se != cpfp(0)) ? 1 : 0;
    outbuf_relu[k][i].sf = (val.sf != cpfp(0)) ? 1 : 0;
  }
}

void relu_bw(cpfp16 *inbuf, char16 *inbuf_relu, int inbuf_off, int num_iter) {
  RELU_BW: for (int i = 0; i < num_iter; ++i) {
  #pragma HLS dependence variable=inbuf inter false
  #pragma HLS pipeline
    cpfp16 val = inbuf[inbuf_off + i];
    char16 relu_val = inbuf_relu[i];
    inbuf[inbuf_off + i].s0 = (relu_val.s0 == 1) ? val.s0 : cpfp(0);
    inbuf[inbuf_off + i].s1 = (relu_val.s1 == 1) ? val.s1 : cpfp(0);
    inbuf[inbuf_off + i].s2 = (relu_val.s2 == 1) ? val.s2 : cpfp(0);
    inbuf[inbuf_off + i].s3 = (relu_val.s3 == 1) ? val.s3 : cpfp(0);
    inbuf[inbuf_off + i].s4 = (relu_val.s4 == 1) ? val.s4 : cpfp(0);
    inbuf[inbuf_off + i].s5 = (relu_val.s5 == 1) ? val.s5 : cpfp(0);
    inbuf[inbuf_off + i].s6 = (relu_val.s6 == 1) ? val.s6 : cpfp(0);
    inbuf[inbuf_off + i].s7 = (relu_val.s7 == 1) ? val.s7 : cpfp(0);
    inbuf[inbuf_off + i].s8 = (relu_val.s8 == 1) ? val.s8 : cpfp(0);
    inbuf[inbuf_off + i].s9 = (relu_val.s9 == 1) ? val.s9 : cpfp(0);
    inbuf[inbuf_off + i].sa = (relu_val.sa == 1) ? val.sa : cpfp(0);
    inbuf[inbuf_off + i].sb = (relu_val.sb == 1) ? val.sb : cpfp(0);
    inbuf[inbuf_off + i].sc = (relu_val.sc == 1) ? val.sc : cpfp(0);
    inbuf[inbuf_off + i].sd = (relu_val.sd == 1) ? val.sd : cpfp(0);
    inbuf[inbuf_off + i].se = (relu_val.se == 1) ? val.se : cpfp(0);
    inbuf[inbuf_off + i].sf = (relu_val.sf == 1) ? val.sf : cpfp(0);
  }
}

void input_stage(cpfp16 inbuf[8 * 256 * 16], unsigned short ksize,
    unsigned short xt_off, unsigned short xtile_pad, unsigned short yt_off, 
    unsigned short row_off, unsigned short ydim, unsigned short xdim, 
    unsigned short w_off, unsigned short burstchannels, int image_off,
    bool fc, int numimages, bool backward_flag, cpfp it[16][3]) {
  unsigned short p, q, j, toff;

  cpfp tempbuf[34];
#pragma HLS ARRAY_PARTITION variable=tempbuf complete 
  
  short crow_off;
  unsigned short c_off = (fc) ? 0 : (ksize == 5) ? w_off >> 1 :
    (ksize == 1) ? w_off << 1 : w_off;
  unsigned short flag = w_off & 0x1;

  crow_off = (ksize >> 1) - row_off;
 
  unsigned short pad = 2;
  short off = 0;
  int fc_in_idx = (backward_flag) ? xt_off * (numimages >> 4) + image_off :
    image_off * (xtile_pad >> 3) + xt_off;
  int conv_in_idx = ((((image_off * burstchannels + c_off) * ydim +
    (yt_off - crow_off)) * xtile_pad * 2) >> 4) + xt_off;

  int in_idx = (fc) ? fc_in_idx : conv_in_idx;

  int channel_off = (ksize == 1) ? (ydim * xtile_pad * 2) >> 4 : -1;

  if (yt_off - crow_off >= 0 && yt_off - crow_off < ydim && 
      c_off < burstchannels) {
    tempbuf[2] = inbuf[in_idx].s0;
    tempbuf[3] = inbuf[in_idx].s1;
    tempbuf[4] = inbuf[in_idx].s2;
    tempbuf[5] = inbuf[in_idx].s3;
    tempbuf[6] = inbuf[in_idx].s4;
    tempbuf[7] = inbuf[in_idx].s5;
    tempbuf[8] = inbuf[in_idx].s6;
    tempbuf[9] = inbuf[in_idx].s7;
    tempbuf[10] = inbuf[in_idx].s8;
    tempbuf[11] = inbuf[in_idx].s9;
    tempbuf[12] = inbuf[in_idx].sa;
    tempbuf[13] = inbuf[in_idx].sb;
    tempbuf[14] = inbuf[in_idx].sc;
    tempbuf[15] = inbuf[in_idx].sd;
    tempbuf[16] = inbuf[in_idx].se;
    tempbuf[17] = inbuf[in_idx].sf;
    if ((ksize == 1) && !fc && c_off + 1 < burstchannels) {
      tempbuf[18] = inbuf[in_idx + channel_off].s0;
      tempbuf[19] = inbuf[in_idx + channel_off].s1;
      tempbuf[20] = inbuf[in_idx + channel_off].s2;
      tempbuf[21] = inbuf[in_idx + channel_off].s3;
      tempbuf[22] = inbuf[in_idx + channel_off].s4;
      tempbuf[23] = inbuf[in_idx + channel_off].s5;
      tempbuf[24] = inbuf[in_idx + channel_off].s6;
      tempbuf[25] = inbuf[in_idx + channel_off].s7;
      tempbuf[26] = inbuf[in_idx + channel_off].s8;
      tempbuf[27] = inbuf[in_idx + channel_off].s9;
      tempbuf[28] = inbuf[in_idx + channel_off].sa;
      tempbuf[29] = inbuf[in_idx + channel_off].sb;
      tempbuf[30] = inbuf[in_idx + channel_off].sc;
      tempbuf[31] = inbuf[in_idx + channel_off].sd;
      tempbuf[32] = inbuf[in_idx + channel_off].se;
      tempbuf[33] = inbuf[in_idx + channel_off].sf;
    } else if (fc) {
      for (int i = 0; i < 16; ++i)
        tempbuf[18 + i] = 0;
    } else {
      if (xt_off == 0) {
        tempbuf[0] = 0;
        tempbuf[1] = 0;
      } else {
        tempbuf[0] = inbuf[in_idx + channel_off].se;
        tempbuf[1] = inbuf[in_idx + channel_off].sf;
      }

      if (xt_off * 8 + 8 == xtile_pad) {
        tempbuf[18] = 0;
        tempbuf[19] = 0;
      } else {
        tempbuf[18] = inbuf[in_idx + 1].s0;
        tempbuf[19] = inbuf[in_idx + 1].s1;
      }
      tempbuf[20] = 0;
    } 
  } else {
    for (p = 0; p < 34; ++p) 
      tempbuf[p] = 0;
  }

  if (ksize != 5)
    toff = 1;
  else if (flag == 0)
    toff = 0;
  else
    toff = 3;

  for (p = 0; p < 3; ++p) {
    for (q = 0; q < 16; ++q) {
      if (fc || (ksize == 1)) {
        if (p == 0)
          it[q][p] = tempbuf[q + 2];
        else if (p == 1 && !fc)
          it[q][p] = tempbuf[q + 18];
        else
          it[q][p] = 0;
      } else {
        if (p + q + toff + xt_off * 16 < xdim + pad)
          it[q][p] = tempbuf[p + q + toff];
        else
          it[q][p] = 0;
      }
    }
  }
}

void wt_set(cpfp16 wbuf[OCFACT][512], cpfp wt[OCFACT][16][3], 
    unsigned short w_off, unsigned short row_off, unsigned short ksize,
    unsigned short xt_off, unsigned short xdim, bool backward_flag, bool fc) {
  cpfp wvals[16]; 
#pragma HLS ARRAY_PARTITION variable=wvals complete dim=1

  cpfp it[3];
  unsigned short weight_idx = (ksize == 1 && !backward_flag && !fc) ?
    w_off >> 3 : w_off;
  for (int k = 0; k < OCFACT; ++k) {
    wvals[0] = wbuf[k][weight_idx].s0;
    wvals[1] = wbuf[k][weight_idx].s1;
    wvals[2] = wbuf[k][weight_idx].s2;
    wvals[3] = wbuf[k][weight_idx].s3;
    wvals[4] = wbuf[k][weight_idx].s4;
    wvals[5] = wbuf[k][weight_idx].s5;
    wvals[6] = wbuf[k][weight_idx].s6;
    wvals[7] = wbuf[k][weight_idx].s7;
    wvals[8] = wbuf[k][weight_idx].s8;
    wvals[9] = wbuf[k][weight_idx].s9;
    wvals[10] = wbuf[k][weight_idx].sa;
    wvals[11] = wbuf[k][weight_idx].sb;
    wvals[12] = wbuf[k][weight_idx].sc;
    wvals[13] = wbuf[k][weight_idx].sd;
    wvals[14] = wbuf[k][weight_idx].se;
    wvals[15] = wbuf[k][weight_idx].sf;
    if (ksize != 1) {
      it[0] = wvals[row_off * 3 + 0];
      it[1] = wvals[row_off * 3 + 1];
      it[2] = wvals[row_off * 3 + 2];
    } else {
      it[0] = wvals[(w_off & 0x7) * 2];
      it[1] = wvals[(w_off & 0x7) * 2 + 1];
      it[2] = 0;
    }
    for (int p = 0; p < 16; ++p) {
      for (int q = 0; q < 3; ++q) {
        if (fc) {
          wt[k][p][q] = wvals[p];
        } else {
          if (backward_flag) {
            if (p + xt_off * 16 < xdim) {
              wt[k][p][q] = wvals[p];
            } else {
              wt[k][p][q] = 0;
            }
          } else {
            wt[k][p][q] = it[q];
          }
        }
      }
    } 
  }
}

/* Kernel used for computing direct convolution forward and backward. 
 * input:         flattened input array containing image data, padded to be
 *                divisible by 16 on the x dimension
 * weights:       3x3 filters, padded to be size 16
 * bias:          flattened bias array
 * output:        output of the convolution, padded to be divisible by 16 on 
 *                the x dimension
 * group_idx:     group_idx index, leave as 0 if not using group convolution
 * image_idx:     image offset
 */ 

extern "C" {

void cr_layer_fb_cpfp(cpfp16 *input, cpfp16 *weights, cpfp *bias,
    cpfp16 *output, char16 *track_relu, int *params, int group_idx,
    int image_idx) { 
// Ports 
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=output
#pragma HLS data_pack variable=input
#pragma HLS data_pack variable=track_relu
#pragma HLS INTERFACE m_axi port=input offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi port=weights offset=slave bundle=gmem3
#pragma HLS INTERFACE m_axi port=bias offset=slave bundle=gmem4
#pragma HLS INTERFACE m_axi port=track_relu offset=slave bundle=gmem5
#pragma HLS INTERFACE m_axi port=params offset=slave bundle=gmem6
#pragma HLS INTERFACE s_axilite port=input bundle=control
#pragma HLS INTERFACE s_axilite port=output bundle=control
#pragma HLS INTERFACE s_axilite port=weights bundle=control
#pragma HLS INTERFACE s_axilite port=bias bundle=control
#pragma HLS INTERFACE s_axilite port=track_relu bundle=control
#pragma HLS INTERFACE s_axilite port=params bundle=control

#pragma HLS INTERFACE s_axilite port=group_idx bundle=control
#pragma HLS INTERFACE s_axilite port=image_idx bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

  // Input tile buffer
  cpfp16 inbuf[8 * 256 * 16];
  char16 inbuf_relu[256 * 16];
  char16 outbuf_relu[OCFACT][512];

  // Output buffer used for writing
  cpfp16 outbuf[OCFACT][512];
#pragma HLS ARRAY_PARTITION variable=outbuf complete dim=1

  // Weight buffer
  cpfp16 wbuf[OCFACT][512];
#pragma HLS ARRAY_PARTITION variable=wbuf complete dim=1

  // Bias buffer
  cpfp biasbuf[1024];
DO_PRAGMA(HLS ARRAY_PARTITION variable=biasbuf cyclic factor=OCFACT)

  // Input tile registers post transform
  cpfp it[16][3];
#pragma HLS ARRAY_PARTITION variable=it complete dim=1
#pragma HLS ARRAY_PARTITION variable=it complete dim=2

  // Temporary output tile registers
  cpfp ot[OCFACT][16][3];
#pragma HLS ARRAY_PARTITION variable=ot complete dim=1
#pragma HLS ARRAY_PARTITION variable=ot complete dim=2
#pragma HLS ARRAY_PARTITION variable=ot complete dim=3

  cpfp wt[OCFACT][16][3];
#pragma HLS ARRAY_PARTITION variable=wt complete dim=1
#pragma HLS ARRAY_PARTITION variable=wt complete dim=2
#pragma HLS ARRAY_PARTITION variable=wt complete dim=3

  // Ouput tile transform stage 1 output
  cpfp ot_s1[OCFACT][24];
#pragma HLS ARRAY_PARTITION variable=ot_s1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=ot_s1 complete dim=2

  cpfp ot_s2[OCFACT][3][4];
#pragma HLS ARRAY_PARTITION variable=ot_s2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=ot_s2 complete dim=2
#pragma HLS ARRAY_PARTITION variable=ot_s2 complete dim=3

  cpfp ot_s3[OCFACT][3][2];
#pragma HLS ARRAY_PARTITION variable=ot_s3 complete dim=1
#pragma HLS ARRAY_PARTITION variable=ot_s3 complete dim=2
#pragma HLS ARRAY_PARTITION variable=ot_s3 complete dim=3

  cpfp ot_s4[OCFACT][3];
#pragma HLS ARRAY_PARTITION variable=ot_s4 complete dim=1
#pragma HLS ARRAY_PARTITION variable=ot_s4 complete dim=2
 
  cpfp otf[OCFACT][16];
#pragma HLS ARRAY_PARTITION variable=otf complete dim=1
#pragma HLS ARRAY_PARTITION variable=otf complete dim=2

  cpfp otfc[OCFACT][16];
#pragma HLS ARRAY_PARTITION variable=otfc complete dim=1
#pragma HLS ARRAY_PARTITION variable=otfc complete dim=2

  cpfp otb[OCFACT][16];
#pragma HLS ARRAY_PARTITION variable=otb complete dim=1
#pragma HLS ARRAY_PARTITION variable=otb complete dim=2

  short inchannels = params[0];
  short outchannels = params[1];
  short burstchannels = params[2];
  short rpo = params[3];
  short rpofm = params[4];
  short burstydim = params[5];
  short ydim = params[6];
  short xdim = params[7];
  short xtile_pad = params[8];
  short ksize = params[9];
  short numgroups = params[10];
  short numimages = params[11];
  short fc = params[12];
  short relu = params[13];
  short backward = params[14];
  assert(rpo >= 1);
  assert(rpo <= 64);

  assert(rpofm <= 32);
  assert(rpofm >= 1);

  assert(burstydim <= 64);
  assert(burstydim >= 1);

  assert(ksize == 1 || ksize == 3 || ksize == 5);

  assert(numimages >= 1);

  assert((backward == 0) || (backward == 1) || (backward == 2) ||
    (backward == 3));

  unsigned short w_off;
  unsigned short row_off, xt_off, yt_off, fc_off, i_off;
  unsigned short fact = xtile_pad >> 3;
  unsigned short out_size, weight_size;
  int out_offset;  
  int weight_offset;
  unsigned short offset;
  int in_off, inbuf_off, in_size;
  unsigned short iter, outer_iters;
  bool backward_flag = (backward == 1);
  bool fc_flag = (fc == 1);
  unsigned short mod_channel;
  unsigned short temp_inchannels = (ksize != 1) ? inchannels :
    (backward_flag) ? inchannels >> 1 : (inchannels % 16 == 0) ?
    inchannels >> 4 : (inchannels >> 4) + 1;

  if (backward_flag) {
    if (ksize == 5) {
      mod_channel = (burstchannels * 2 * ksize < HADD_LATENCY) ? HADD_LATENCY :
        burstchannels * 2;
    } else if (ksize == 1) {
      mod_channel = ((burstchannels >> 1) * ksize < HADD_LATENCY) ?
        HADD_LATENCY : burstchannels >> 1;
    } else {
      mod_channel = (burstchannels * ksize < HADD_LATENCY) ? HADD_LATENCY :
        burstchannels;
    }
  } else {
    mod_channel = (ksize == 5) ? burstchannels * 2 :
      (ksize == 1) ? burstchannels >> 1 : burstchannels;
  }

  unsigned short mod_ydim;
  mod_ydim = ((burstydim < HADD_LATENCY) && !(backward_flag)) ? HADD_LATENCY :
    burstydim; 
  // Read bias data into buffer 
  int bias_offset = outchannels * group_idx;
  int bias_size = outchannels;

  memcpy(biasbuf, bias + bias_offset, sizeof(cpfp) * bias_size);

  int parallel_off = (backward_flag) ? (numimages >> 4) * xtile_pad * 2 :
    numimages * fact;

  int mac_iterations = (fc_flag) ? parallel_off : mod_channel * mod_ydim *
    fact * ksize;
  int numimages_iter = (fc_flag) ? 1 : numimages; 
  outer_iters = (outchannels % OCFACT != 0) ? (outchannels / OCFACT) + 1 : 
    (outchannels / OCFACT);

  for (int n = 0; n < rpo; ++n) {
    // Read the input
    for (int image_off = 0; image_off < numimages; ++image_off) {
      in_off = (((image_idx * numimages + image_off) * numgroups + group_idx)
        * inchannels) * ydim * fact + n * burstchannels * ydim * fact;
      inbuf_off = image_off * burstchannels * ydim * fact;
      in_size = burstchannels * ydim * fact;
      memcpy(inbuf + inbuf_off, input + in_off, sizeof(cpfp16) * in_size);
      if (relu && ((backward == 1) || (backward == 2) || (backward == 3))) {
        memcpy(inbuf_relu, track_relu + in_off, sizeof(char16) * in_size);
        relu_bw(inbuf, inbuf_relu, inbuf_off, in_size);
      } 
    }
    for (int o = 0; o < outer_iters; ++o) {
      for (int image_off = 0; image_off < numimages_iter; ++image_off) {
        for (int offy = 0; offy < rpofm; ++offy) {
          if (n == 0 && !backward_flag) {
            out_size = (fc_flag) ? (numimages >> 4) : burstydim * fact;
            for (int i = 0; i < out_size; ++i) {
            #pragma HLS pipeline
              for (int k = 0; k < OCFACT; ++k) {
                cpfp16 bias_;
                bias_ = biasbuf[o * OCFACT + k];
                outbuf[k][i] = bias_;
              }
            } 
          } else if (!backward_flag && !fc_flag) {
            for (int k = 0; k < OCFACT; ++k) {
              out_offset = (image_idx * numimages + image_off) * numgroups *
                outchannels * ydim * fact + ((o * OCFACT + k + outchannels *
                group_idx) * ydim + offy * burstydim) * fact;
              out_size = fact * burstydim;
              memcpy(outbuf[k], output + out_offset, sizeof(cpfp16) *
                  out_size);
            }
          } else if ((offy == 0) && (image_off == 0)) {
            for (int k = 0; k < OCFACT; ++k) {
              if (fc) {
                out_offset = (o * OCFACT + k) * fact;
                out_size = fact;
              } else {
                out_size = (ksize != 1) ? burstchannels : burstchannels >> 1;
                out_offset = (o * OCFACT + k + outchannels * group_idx) *
                  temp_inchannels + n * out_size;
              
                if (ksize == 5) {
                  out_offset = out_offset << 1;
                  out_size = out_size << 1;
                }
              }
              memcpy(outbuf[k], output + out_offset, sizeof(cpfp16) *
                  out_size);
            } 
          }
          for (int k = 0; k < OCFACT; ++k) {
            int weight_offset_b = (image_idx * numimages + image_off) *
              numgroups * outchannels * ydim * fact + ((o * OCFACT + k +
              outchannels * group_idx) * ydim + offy * burstydim) * fact;
            int weight_size_b = fact * burstydim;
            int weight_size_f = (ksize != 1) ? burstchannels :
              (burstchannels % 16 == 0) ? burstchannels >> 4 :
              (burstchannels >> 4) + 1;
            int weight_offset_f = (o * OCFACT + k + outchannels * group_idx) *
              temp_inchannels + n * weight_size_f;
            int weight_offset_fc_f = (o * OCFACT + k) * fact;
            int weight_size_fc_f = fact;
            int weight_offset_fc_b = (o * OCFACT + k) * (numimages >> 4);
            int weight_size_fc_b = numimages >> 4;
            if (ksize == 5) {
              weight_offset_f = weight_offset_f << 1;
              weight_size_f = weight_size_f << 1;
            }
            if (backward_flag) {
              if (fc_flag) {
                weight_offset = weight_offset_fc_b;
                weight_size = weight_size_fc_b;
              } else {
                weight_offset = weight_offset_b;
                weight_size = weight_size_b;
              }
            } else {
              if (fc_flag) {
                weight_offset = weight_offset_fc_f;
                weight_size = weight_size_fc_f;
              } else {
                weight_offset = weight_offset_f;
                weight_size = weight_size_f;
              }
            }
            if ((!backward_flag && (offy == 0) && image_off == 0) ||
              backward_flag)
              memcpy(wbuf[k], weights + weight_offset, sizeof(cpfp16) *
                weight_size);
          }
          
          w_off = 0;
          xt_off = 0;
          yt_off = 0;
          row_off = 0;
          iter = 0;
          fc_off = 0;
          i_off = 0;
          MULTACCSTAGE: for (int i = 0; i < mac_iterations; ++i, ++iter,
            ++fc_off) {
          #pragma HLS DEPENDENCE variable=outbuf inter false
          #pragma HLS DEPENDENCE variable=wbuf inter false
          #pragma HLS DEPENDENCE variable=otb inter false
          #pragma HLS pipeline        
            if (fc_off == 16) {
              fc_off = 0;
            }
            if (backward_flag) {
              if (fc_flag) {
                if (iter == xtile_pad * 2) {
                  i_off++;
                  iter = 0;
                }
                xt_off = iter;
                w_off = 0;
                yt_off = 0;
                row_off = 0;
              } else {
                if (iter == ksize) {
                  iter = 0;
                  if (w_off == mod_channel - 1) {
                    w_off = 0;
                    if (xt_off == fact - 1) {
                      xt_off = 0;
                      yt_off++;
                    } else {
                      xt_off++;
                    }
                  } else {
                    w_off++;
                  }
                }
                row_off = iter;
              }
            } else {
              if (fc_flag) {
                if (iter == numimages) {
                  xt_off++;
                  iter = 0;
                }
                w_off = xt_off;
                i_off = iter;
                yt_off = 0;
                row_off = 0;
              } else {
                if (iter == fact) {
                  iter = 0;
                  if (yt_off == mod_ydim - 1) {
                    yt_off = 0;
                    if (row_off == ksize - 1) {
                      row_off = 0;
                      w_off++;
                    } else {
                      row_off++;
                    }
                  } else {
                    yt_off++;
                  }
                }
                xt_off = iter;
              }
            } 

            offset = yt_off * fact + xt_off;

            unsigned short fc_w_idx = (backward_flag) ? i_off : w_off;
            unsigned short fc_o_idx = (backward_flag) ? (xt_off >> 4) :
              (i_off >> 4);

            unsigned short conv_w_idx = (backward_flag) ? offset : w_off;
            unsigned short conv_o_idx = (backward_flag) ? w_off : offset;
            unsigned short o_idx = (fc_flag) ? fc_o_idx : conv_o_idx;
            unsigned short w_idx = (fc_flag) ? fc_w_idx : conv_w_idx;

            unsigned short in_image_off = (fc_flag) ? i_off : image_off;

            bool acc_enable = (((backward_flag && (row_off == ksize - 1)) ||
              !backward_flag) && !fc_flag) || (fc_flag && fc_off == 15);

            input_stage(inbuf, ksize, xt_off, xtile_pad, yt_off + offy *
              burstydim, row_off, ydim, xdim, w_off, burstchannels,
              in_image_off, fc_flag, numimages, backward_flag, it);
            
            wt_set(wbuf, wt, w_idx, row_off, ksize, xt_off, xdim,
              backward_flag, fc_flag); 

            for (int k = 0; k < OCFACT; ++k) {
              for (int p = 0; p < 16; ++p) {
                for (int q = 0; q < 3; ++q) {
                  ot[k][p][q] = it[p][q] * wt[k][p][q];
                }
              }
              for (int p = 0; p < 8; ++p) 
                ot_s1[k][p + 2 * 8] = ot[k][p * 2][2] + ot[k][p * 2 + 1][2];

              if (backward_flag || fc_flag) {
                for (int q = 0; q < 2; ++q) {
                  for (int p = 0; p < 8; ++p) 
                    ot_s1[k][p + q * 8] = ot[k][p * 2][q] +
                      ot[k][p * 2 + 1][q];
                }
                for (int q = 0; q < 3; ++q) {
                  for (int p = 0; p < 4; ++p)
                    ot_s2[k][q][p] = ot_s1[k][p * 2 + q * 8] +
                      ot_s1[k][p * 2 + 1 + q * 8];
                }
              } else {
                for (int p = 0; p < 16; ++p) {
                  ot_s1[k][p] = ot[k][p][0] + ot[k][p][1] + ot[k][p][2];
                }
              }

              for (int q = 0; q < 3; ++q) {
                for (int p = 0; p < 2; ++p)
                  ot_s3[k][q][p] = ot_s2[k][q][p * 2] + ot_s2[k][q][p * 2 + 1];
                ot_s4[k][q] = ot_s3[k][q][0] + ot_s3[k][q][1];
              }
               
              for (int p = 0; p < 3; ++p)
                otb[k][row_off * 3 + p] = ot_s4[k][p];

              otfc[k][fc_off] = ot_s4[k][0];

              for (int p = 0; p < 16; ++p) {
                if (fc_flag) {
                  otf[k][p] = otfc[k][p];
                } else {
                  if (backward_flag) {
                    otf[k][p] = otb[k][p];
                  } else {
                    otf[k][p] = ot_s1[k][p];
                  }
                }
              }
              if (acc_enable) {
                outbuf[k][o_idx].s0 += otf[k][0];
                outbuf[k][o_idx].s1 += otf[k][1];
                outbuf[k][o_idx].s2 += otf[k][2];
                outbuf[k][o_idx].s3 += otf[k][3];
                outbuf[k][o_idx].s4 += otf[k][4];
                outbuf[k][o_idx].s5 += otf[k][5];
                outbuf[k][o_idx].s6 += otf[k][6];
                outbuf[k][o_idx].s7 += otf[k][7];
                outbuf[k][o_idx].s8 += otf[k][8];
                outbuf[k][o_idx].s9 += otf[k][9];
                outbuf[k][o_idx].sa += otf[k][10];
                outbuf[k][o_idx].sb += otf[k][11];
                outbuf[k][o_idx].sc += otf[k][12];
                outbuf[k][o_idx].sd += otf[k][13];
                outbuf[k][o_idx].se += otf[k][14];
                outbuf[k][o_idx].sf += otf[k][15];
              }
            }
          }

          for (int k = 0; k < OCFACT; ++k) {
            if (backward_flag) {
              if (fc_flag) {
                out_offset = (o * OCFACT + k) * fact;
                out_size = fact;
              } else {
                out_size = (ksize != 1) ? burstchannels : burstchannels >> 1;
                out_offset = (o * OCFACT + k + outchannels * group_idx) *
                  temp_inchannels + n * out_size;
                out_size = out_size;

                if (ksize == 5) {
                  out_offset = out_offset << 1;
                  out_size = out_size << 1;
                }
              }
            } else {
              if (fc_flag) {
                out_offset = image_idx * outchannels * (numimages >> 4) +
                  (o * OCFACT + k) * (numimages >> 4);
                out_size = numimages >> 4;
              } else {
                out_offset = (image_idx * numimages + image_off) * numgroups *
                  outchannels * ydim * fact + ((o * OCFACT + k + outchannels *
                  group_idx) * ydim + offy * burstydim) * fact;
                out_size = fact * burstydim;
              }
              if (relu && (o * OCFACT + k < outchannels) && (backward != 2) &&
                (backward != 3)) {
                relu_fw(outbuf, outbuf_relu, k, out_size);
                memcpy(track_relu + out_offset, outbuf_relu[k],
                  sizeof(char16) * out_size);
              }
            }

            if ((o * OCFACT + k < outchannels &&
              ((backward_flag && (offy == rpofm - 1) && 
              (image_off == numimages_iter - 1)) || !backward_flag))) {
              memcpy(output + out_offset, outbuf[k], sizeof(cpfp16) *
                out_size);
            }
          }
        }
      }
    }
  }
}

}
