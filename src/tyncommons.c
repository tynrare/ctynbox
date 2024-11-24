#include "include/tyncommons.h"
#include <stdarg.h>
#include "raylib.h"
#include <math.h>

float AngleDifference(float angle1, float angle2) {
  double diff = ((int)angle2 - (int)angle1 + 180) % 360 - 180;
  return diff < -180 ? diff + 360 : diff;
}

bool isAnyKeyPressed(int count, ...) {
  bool pressed = false;

  va_list args;
  va_start(args, count);

  for (int i = 0; i < count; i++) {
    if (IsKeyDown(va_arg(args, int))) {
      pressed = true;
    }
  }

  va_end(args);

  return pressed;
}

int min(int a, int b) { return a > b ? b : a; }

float lerp(float a, float b, float t) { return a + (b - a) * t; }

float rlerp(float a, float b, float t) {
  float CS = (1.0f - t) * cosf(a) + t * cosf(b);
  float SN = (1.0f - t) * sinf(a) + t * sinf(b);

  return atan2f(SN, CS);
}

float dlerp(float a, float b, float decay, float dt) {
	return b + (a - b) * expf(-decay * dt);
}
