#pragma once

void quat2Euler(float *q, float *euler_out, const float magDeclination);
void normalize(float *, int);
void normalizeQuat(float *q);
float norm2(float *in, const int len);
void inv2(float M[2][2]);
void quatAdd(float*, float*, float*);
void quatProd(float*, float*, float*);
void quatProdConst(float*, float, float*);
void matProd_4x6x1(float *in1, float *in2, float *out);
void matProd_3x3x1(float *in1, float *in2, float *out);
void matProd_2x2x2(float *in1, float *in2, float *out);
void matProd_2x2x1(float *in1, float *in2, float *out);
void quatConj(float *in, float *out);
