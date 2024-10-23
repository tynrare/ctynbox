#include "raylib.h"
#include "raymath.h"
#include "src/include/app.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

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
//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main() {
    const int seed = 2;

  // Initialization
  //--------------------------------------------------------------------------------------

    // SetConfigFlags(FLAG_MSAA_4X_HINT); // Set MSAA 4X hint before windows
    //  creation

  InitWindow(viewport_w, viewport_h, "ctynbox");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);

  state = AppInit(&stage);
  SetRandomSeed(seed);

  loop();

  stage.frame.dispose(state);

  return 0;
}

static void loop() {
  #if defined(PLATFORM_WEB)
    emscripten_set_main_loop(step, 0, 1);
  #else

    while (!WindowShouldClose()) {
      step();
    }
  #endif
}

static void step() {
    STAGEFLAG flags = stage.frame.step(state, stage.flags);

    if (flags & STAGEFLAG_DISABLED) {
      return;
    }

    BeginDrawing();

    stage.frame.draw(state);

    EndDrawing();
}


