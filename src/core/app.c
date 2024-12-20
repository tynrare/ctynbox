#include "../include/app.h"
#include "../include/game_spaceexp.h"
#include "../include/draft0.h"
#include "../include/editor0.h"
#include "../include/game_tynmaze.h"
#include "../include/test_networksim_0.h"
#include "../include/test_render_0.h"
#include "../include/test_shader_0.h"
//#include "../include/test_physics_0.h"
#include "../include/test_collisions_0.h"
#include "../include/game_platformer_0.h"
#include "../include/test_tynmem.h"
#include "../include/demo_boids0.h"

#include "../include/tyncommons.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

void AppDispose(AppState *state);
static char *cmd(AppState *state, char *command, STAGEFLAG *flag);
STAGEFLAG AppStep(AppState *state, STAGEFLAG flag);
void AppDraw(AppState *state);
void AppNewStage(AppState *state, void *(initfn)(TynStage *));
int AppPushStage(AppState *state, TynStage *stage);

AppState *AppInit(TynStage *stage) {
  AppState *state = malloc(sizeof(AppState));
  state->activestages = 0;

  stage->frame = (TynFrame){&AppDispose, &AppStep, &AppDraw};

  AppNewStage(state, Console_Init);
  //AppNewStage(state, GSpaceexp_Init);
  //AppNewStage(state, TestTynmemInit);
  AppNewStage(state, DemoBoids0Init);
  //AppNewStage(state, draft0_init);
  //AppNewStage(state, editor0_init);
  //AppNewStage(state, GamePlatformer0Init);
  //AppNewStage(state, TestShader0Init);
  //AppNewStage(state, TestRender0Init);
  // AppNewStage(state, TynmazeInit);
  // AppNewStage(state, TestPhysics0Init);
  // AppNewStage(state, TestCollisions0Init);
  //AppNewStage(state, TestNetworksim0Init);

  return state;
}

void AppNewStage(AppState *state, void *(initfn)(TynStage *)) {
  TynStage *stage = malloc(sizeof(TynStage));
  stage->flags = 0;
  // stage->flags = 0;
  initfn(stage);
  AppPushStage(state, stage);
}

int AppPushStage(AppState *state, TynStage *stage) {
  state->stages[state->activestages] = stage;

  return state->activestages++;
}

int AppPopStage(AppState *state) {
  TynStage *stage = state->stages[--state->activestages];

  if (stage->frame.dispose) {
    stage->frame.dispose(stage->state);
  } else {
    free(stage->state);
  }

  free(stage);

  return state->activestages;
}

void AppDispose(AppState *state) {
  while (state->activestages) {
    AppPopStage(state);
  }

  free(state);
}

void AppCleanupStages(AppState *state) {
  while (state->activestages > 1)
    AppPopStage(state);
}

static char *cmd(AppState *state, char *command, STAGEFLAG *flags) {
  if (strcmp(command, "exit") == 0) {
    CloseWindow();
    *flags |= STAGEFLAG_DISABLED;
  } else if (strcmp(command, "?") == 0) {
    return "\ninfo commands:\n"
    " > ? - this message\n"
    " > ?? - current state info message\n"
    " > ?time\n > ?play\n > ?maze\n"
    "\naction commands:\n"
    " > close stage \n > exit\n";
  } else if (strcmp(command, "?play") == 0) {
    return "type: 'cd X'; where X:\n"
            " game0, maze, shadertest0, \n networktest0, collisiontest0, platformer0,\n "
						"memtest0, boids0.\n"        
           "\ndisabled:\n editor\n rendertest0";
  } else if (strcmp(command, "?time") == 0) {
    return "4:20";
  } else if (strcmp(command, "cd editor") == 0) {
    AppCleanupStages(state);
    //AppNewStage(state, editor0_init);
    return "first actual demo here";
  } else if (strcmp(command, "cd game0") == 0) {
    AppCleanupStages(state);
    AppNewStage(state, GSpaceexp_Init);
    return "first actual demo here";
  } else if (strcmp(command, "?maze") == 0) {
    return "\nmaze commands (in [cd maze] mode):\n"
    " mode fp, mode topdown, mode free\n";
  } else if (strcmp(command, "cd maze") == 0) {
    AppCleanupStages(state);
    AppNewStage(state, TynmazeInit);
    return "Maze game";
  } else if (strcmp(command, "cd shadertest0") == 0) {
    AppCleanupStages(state);
    AppNewStage(state, TestShader0Init);
    return "Shader with game props data stored in texture";
  } else if (strcmp(command, "cd networktest0") == 0) {
    AppCleanupStages(state);
    AppNewStage(state, TestNetworksim0Init);
    return "wip network movement";
  } else if (strcmp(command, "cd rendertest0") == 0) {
    AppCleanupStages(state);
    //AppNewStage(state, TestRender0Init);
    return "wip render test";
  } else if (strcmp(command, "cd memtest0") == 0) {
    AppCleanupStages(state);
		AppNewStage(state, TestTynmemInit);
    return "memory pools, trees test";
  } else if (strcmp(command, "cd collisiontest0") == 0) {
    AppCleanupStages(state);
    AppNewStage(state, TestCollisions0Init);
    return "wip physics test";
  } else if (strcmp(command, "cd platformer0") == 0) {
    AppCleanupStages(state);
    AppNewStage(state, GamePlatformer0Init);
    return "platformer test";
  } else if (strcmp(command, "cd boids0") == 0) {
    AppCleanupStages(state);
    AppNewStage(state, DemoBoids0Init);
    return "platformer test";
  } else if (strcmp(command, "close stage ..") == 0) {
    AppCleanupStages(state);
    return "stage closed";
  }

  return NULL;
}

STAGEFLAG AppStep(AppState *state, STAGEFLAG flags) {
  // writes any broadcasted message from stages
  char *broadcastcmd = NULL;
  // disables further stepping if any stage blocks it
  bool blockstep = false;
  for (int i = 0; i < state->activestages; i++) {
    TynStage *stage = state->stages[i];

    // does nothing if stage disabled
    if (stage->flags & STAGEFLAG_DISABLEDSTEP) {
      continue;
    }

    // push broadcast command from previous stages
    if (broadcastcmd && stage->frame.cmdout) {
      stage->frame.cmdout(stage->state, broadcastcmd);
    }

    // does nothing if previous stage blocked further steps
    if (blockstep) {
      continue;
    }

    if (stage->frame.step) {
      stage->flags = stage->frame.step(stage->state, stage->flags);
    }

    // start cmd processing
    if (stage->frame.cmdin) {
      CMDFLAG flag = 0;
      // get command from frame
      char *message = stage->frame.cmdin(stage->state, &flag);
      if (message) {
				TraceLog(LOG_INFO, TextFormat("cmdin: \n%s", message));
        char *response = cmd(state, message, &flags);
        // response to command
        if (response && stage->frame.cmdout) {
					TraceLog(LOG_INFO, TextFormat("cmdout: \n\n%s", response));
          stage->frame.cmdout(stage->state, response);
        }
      }
      // storage broadcas commands
      if (message && flag | CMDFLAG_BROADCAST) {
        broadcastcmd = message;
      }
    }

    blockstep = (bool)(stage->flags & STAGEFLAG_BLOCKSTEP);
  }

  return flags;
}

void AppDraw(AppState *state) {
  ClearBackground(RAYWHITE);

  for (int i = state->activestages - 1; i >= 0; i--) {
    TynStage *stage = state->stages[i];

    if (stage->flags & STAGEFLAG_DISABLEDDRAW) {
      continue;
    }

    if (stage->frame.draw) {
      stage->frame.draw(stage->state);
    }

    if (stage->flags & STAGEFLAG_BLOCKDRAW) {
      break;
    }
  }
}
