#pragma once

void normalizeQuat(float *q);
float norm2(float *in, const int len);
void quatAdd(float*, float*, float*);
void quatProd(float*, float*, float*);
void quatProdConst(float*, float, float*);
void matProd_4x6x1(float *in1, float *in2, float *out);
void quatConj(float *in, float *out);
