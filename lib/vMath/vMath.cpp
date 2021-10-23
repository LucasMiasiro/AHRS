#include "vMath.h"
#include "config.h"
#include <math.h> 
#include "esp_dsp.h"

void normalize(float *q, int len){
    dsps_mulc_f32_ae32(q, q, len, 1/norm2(q, len), 1, 1);
}

void normalizeQuat(float *q){
    quatProdConst(q, 1/norm2(q, 4), q); //TODO: checar se não é problemático
}

float norm2(float *in, const int len){
    float norm;
    dsps_dotprod_f32_ae32(in, in, &norm, len);
    return norm = sqrt(norm);
}

void quatConj(float *in, float *out){
    out[0] = in[0];
    out[1] = -in[1];
    out[2] = -in[2];
    out[3] = -in[3];
}

void matProd_4x6x1(float *in1, float *in2, float *out){
    dspm_mult_f32_ae32(in1, in2, out, 4, 6, 1);
}

void quatAdd(float *in1, float *in2, float *out){
    dsps_add_f32_ae32(in1, in2, out, 4, 1, 1, 1);
}


void quatProd(float *in1, float *in2, float *out){
    *out = *(in1) * (*in2) - *(in1+1) * (*(in2+1))
              - *(in1+2) * (*(in2+2)) - *(in1+3) * (*(in2+3));

    *(out+1) = *(in1) * (*(in2+1)) + *(in1+1) * (*in2)
              + *(in1+2) * (*(in2+3)) - *(in1+3) * (*(in2+2));

    *(out+2) = *(in1) * (*(in2+2)) - *(in1+1) * (*(in2+3))
              + *(in1+2) * (*in2) + *(in1+3) * (*(in2+1));

    *(out+3) = *(in1) * (*(in2+3)) + *(in1+1) * (*(in2+2))
              - *(in1+2) * (*(in2+1)) + *(in1+3) * (*in2);
}

void quatProdConst(float *in, float K, float *out){
    dsps_mulc_f32_ae32(in, out, 4, K, 1, 1);
}
