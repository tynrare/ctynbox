#include "raylib.h"
#include "include/app.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#include <stdio.h>
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------


void SaveProgress(const char *key, const int value) {
#if defined(PLATFORM_WEB)
  char script[256];
  snprintf(script, sizeof(script), "saveProgress('%s', '%d');", key, value);
  emscripten_run_script(script);
#endif
}

const int LoadProgress(const char *key) {
#if defined(PLATFORM_WEB)
  char script[256];
  snprintf(script, sizeof(script), "loadProgress('%s');", key);
  return emscripten_run_script_int(script);
#else
  return 0;
#endif
}

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const int VIEWPORT_W = 512;
static const int VIEWPORT_H = 512;

static int viewport_w = VIEWPORT_W;
static int viewport_h = VIEWPORT_H;

TynStage stage = {0};
AppState *state = {0};

static void step();
static void loop();
bool active = false;

int dpr = 1;

//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main() {
  const int seed = 2;
  // Initialization
  //--------------------------------------------------------------------------------------

  SetConfigFlags(FLAG_WINDOW_HIGHDPI & FLAG_MSAA_4X_HINT); // Set MSAA 4X hint before windows
#if defined(PLATFORM_WEB)
  viewport_w = LoadProgress("viewport_w");
  viewport_h = LoadProgress("viewport_h");
  dpr = LoadProgress("device_pixel_ratio");
#endif

  InitWindow(viewport_w, viewport_h, "ctynbox");
#ifndef PLATFORM_WEB
  SetWindowState(FLAG_WINDOW_RESIZABLE);
#endif
  SetTargetFPS(60);

  SetRandomSeed(seed);

  state = AppInit(&stage);
  active = true;
  // DisableCursor();

  //SetMouseScale(dpr, dpr);
  TraceLog(LOG_INFO, TextFormat("device_pixel_ratio is %d, viewport_w is %d, viewport_h is %d", dpr, viewport_w, viewport_h));

  loop();

  stage.frame.dispose(state);
  CloseWindow();

  return 0;
}

static void loop() {
  #if defined(PLATFORM_WEB)
    emscripten_set_main_loop(step, 0, 1);
  #else
    SetTargetFPS(60); 

    while (!WindowShouldClose() && active) {
      step();
    }
  #endif
}


// --- system

static long resize_timestamp = -1;
static const float resize_threshold = 0.3;
static Vector2 requested_viewport = {VIEWPORT_W, VIEWPORT_H};
static void equilizer() {
#if defined(PLATFORM_WEB)
  const int vw = LoadProgress("viewport_w");
  const int vh = LoadProgress("viewport_h");
#else
  const int vw = GetScreenWidth();
  const int vh = GetScreenHeight();
#endif


  const long now = GetTime();

  // thresholds resizing
  if (requested_viewport.x != vw || requested_viewport.y != vh) {
    requested_viewport.x = vw;
    requested_viewport.y = vh;

    // first resize triggers intantly (important in web build)
    if (resize_timestamp > 0) {
      resize_timestamp = now;
      return;
    }
  }

  // reinits after riseze stops
  const bool resized =
      requested_viewport.x != viewport_w || requested_viewport.y != viewport_h;
  if (resized && now - resize_timestamp > resize_threshold) {
    resize_timestamp = now;
    viewport_w = vw;
    viewport_h = vh;
    SetWindowSize(viewport_w, viewport_h);
    // init();
  }
}   

void step(void)
{
    equilizer();
    STAGEFLAG flags = stage.frame.step(state, stage.flags);

    if (flags & STAGEFLAG_DISABLED || !active) {
      active = false;
#if defined(PLATFORM_WEB)
      emscripten_cancel_main_loop();
#endif
      return;
    }

    BeginDrawing();
    
    stage.frame.draw(state);
    
    EndDrawing();
}
