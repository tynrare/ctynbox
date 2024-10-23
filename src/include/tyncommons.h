#include <stdbool.h>

#ifndef TYNCOMMONS_H
#define TYNCOMMONS_H

bool isAnyKeyPressed(int count, ...);
int min(int a, int b);
float lerp(float a, float b, float t);
float rlerp(float a, float b, float t);
float dlerp(float a, float b, float decay, float dt);
                
#endif
