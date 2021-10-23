#pragma once
#include <iostream>
namespace serialLogger{
void log_example();
void header();
void blank_lines(int);
void ruler();
void logFloat(const float *dataPtr, const int, const char [], const char[]);
void logFloat(const float *dataPtr, const int, const char []);
void logFloat(const float *dataPtr, const int, const int, const char []);
void logFloat(const float *array[], const int, const int, const char []);
};
