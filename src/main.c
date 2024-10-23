#include "raylib.h"
#include "include/app.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const int VIEWPORT_W = 1024;
static const int VIEWPORT_H = 1024;

static int viewport_w = VIEWPORT_W;
static int viewport_h = VIEWPORT_H;

TynStage stage = {0};
AppState *state = {0};

static void step();
static void loop();
bool active = false;

//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main() {
  const int seed = 2;
  // Initialization
  //--------------------------------------------------------------------------------------

  //SetConfigFlags(FLAG_MSAA_4X_HINT); // Set MSAA 4X hint before windows
    //  creation

   InitWindow(viewport_w, viewport_h, "ctynbox");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);

  state = AppInit(&stage);
  SetRandomSeed(seed);

  state = AppInit(&stage);
  active = true;
  // DisableCursor();

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
  const int vw = GetScreenWidth();
  const int vh = GetScreenHeight();

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
    // init();
  }
}

void step(void)
{
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
