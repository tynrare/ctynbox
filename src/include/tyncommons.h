#include <stdbool.h>

#ifndef TYNCOMMONS_H
#define TYNCOMMONS_H

float AngleDifference(float angle1, float angle2);
bool isAnyKeyPressed(int count, ...);
int min(int a, int b);
float lerp(float a, float b, float t);
float rlerp(float a, float b, float t);
float dlerp(float a, float b, float decay, float dt);

#ifdef __DEBUG__
#define RES_PATH "res/"
#else
#define RES_PATH "../res/"
#endif

#endif
