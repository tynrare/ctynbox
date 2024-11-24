#include "../include/demo_boids0.h"

#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vector2 v2up = (Vector2){0, -1};

#define BOIDS_AMOUNT 128

#define BOID_VIEWDIST 100
#define BOID_VIEWANGLE 100
#define BOID_ROT_MAX_SPEED 0.5
#define BOID_ROT_SPEED 0.02
#define BOID_ROT_ACCELERATION 0.3
#define BOID_ACCELERATION 0.03

#define BOID_COHESION_SPEED_FACTOR 2

float BOID_MOVE_SPEED = 2;
// classics boids rules. 0-1
float BOID_ALIGMENT_FACTOR = 0.2;
float BOID_COHESION_FACTOR = 2.8;
float BOID_SEPARATION_FACTOR = 2.7;
float BOID_CENTRIC_FACTOR = 0.5;
float BOID_COLLISION_DAMAGE = 0; // per sec

bool BOID_DEBUG = false;

static void _dispose(DemoBoids0State *state);
static STAGEFLAG _step(DemoBoids0State *state, STAGEFLAG flags);
static void _draw(DemoBoids0State *state);
static void _init(DemoBoids0State *state);
static char *_cmdin(DemoBoids0State *state, STAGEFLAG *flags);
static char *_cmdout(DemoBoids0State *state, char *message);

static float rlerp(float a, float b, float t) {
  float CS = (1.0f - t) * cosf(a) + t * cosf(b);
  float SN = (1.0f - t) * sinf(a) + t * sinf(b);

  return atan2f(SN, CS);
}

static void draw_boid(Memcell *memcell, Memspace *memspace) {
  DemoBoid0 *boid = memcell->point;
	if (boid->energy <= -1) {
		return;
	}

	if (boid->energy <= 0) {
		boid->energy -= 1 * GetFrameTime();
		float e = 1 + boid->energy;
		boid->sprite.color.a = e * 255;
		boid->sprite.scale = e;
		boid->pos->x += boid->direction.x * BOID_MOVE_SPEED * 0.5;
		boid->pos->y += boid->direction.y * BOID_MOVE_SPEED * 0.5;
		SpriteDraw(&boid->sprite);
		return;
	}

  Memcell *neighbors = memspace->contents_direct.first;

  // rotate followind classic boids rules
  float rotation = Vector2Angle(v2up, boid->direction);
  float alignment = 0;
  float cohesion = 0;
  float separation = 0;
  Vector2 cohesion_center = *boid->pos;
  for (Memcell *m = neighbors; m; m = m->next) {
    DemoBoid0 *e = m->point;

		if (e->energy <= 0) {
			continue;
		}

    Vector2 delta = Vector2Subtract(*e->pos, *boid->pos);
    float dlength = Vector2Length(delta);
    float dfactor = dlength / BOID_VIEWDIST;
    if (dfactor > 1 || dfactor == 0) {
      continue;
    }

		if (BOID_COLLISION_DAMAGE && dlength < 8) {
			boid->energy -= BOID_COLLISION_DAMAGE * GetFrameTime();
		}

    Vector2 ndelta = Vector2Normalize(delta);

    float dangle = Vector2Angle(boid->direction, ndelta);
    float dangle_deg = dangle * RAD2DEG;
    if (dangle_deg > BOID_VIEWANGLE * 0.5 ||
        dangle_deg < -BOID_VIEWDIST * 0.5) {
      continue;
    }

    alignment = rlerp(alignment, Vector2Angle(boid->direction, e->direction),
                      1 - dfactor);
    cohesion = rlerp(cohesion, dangle, 1 - dfactor);
    separation = -cohesion;

    cohesion_center = Vector2Lerp(cohesion_center, *e->pos, 1 - dfactor);

    if (BOID_DEBUG) {
      DrawLine(boid->pos->x, boid->pos->y, e->pos->x, e->pos->y, RED);
    }
  }

  if (BOID_DEBUG) {
    Vector2 dir = Vector2Scale(boid->direction, 32);
    DrawLine(boid->pos->x, boid->pos->y, boid->pos->x + dir.x,
             boid->pos->y + dir.y, BLUE);
    DrawRectangle(cohesion_center.x, cohesion_center.y, 4, 4, GREEN);
  }

	float centric_rotate = 0;
	if (BOID_CENTRIC_FACTOR && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		//Vector2 center = { GetScreenWidth() * 0.5, GetScreenHeight() * 0.5 };
		Vector2 center = GetMousePosition();
		Vector2 delta = Vector2Subtract(center, *boid->pos);
		Vector2 ndelta = Vector2Normalize(delta);
		centric_rotate = Vector2Angle(boid->direction, ndelta);
	}


  // apply rotations
  float rotate = 0;
  // rotate = rlerp(rotate, separation, BOID_SEPARATION_FACTOR);
  // rotate = rlerp(rotate, alignment, BOID_ALIGMENT_FACTOR);
  // rotate = rlerp(rotate, cohesion, BOID_COHESION_FACTOR);
  rotate = rotate + separation * BOID_SEPARATION_FACTOR;
  rotate = rotate + alignment * BOID_ALIGMENT_FACTOR;
  rotate = rotate + cohesion * BOID_COHESION_FACTOR;
  rotate = rotate + centric_rotate * BOID_CENTRIC_FACTOR;
  float rotate_clamped =
      Clamp(rotate * BOID_ROT_SPEED, -BOID_ROT_MAX_SPEED, BOID_ROT_MAX_SPEED);

  boid->torque = rlerp(boid->torque, rotate_clamped, BOID_ROT_ACCELERATION);
  boid->direction = Vector2Rotate(boid->direction, boid->torque);

  // additionaly apply speed tunes
  Vector2 cohesion_delta = Vector2Subtract(*boid->pos, cohesion_center);
  Vector2 ncohesion_delta = Vector2Normalize(cohesion_delta);
  float cohesion_dot = Vector2DotProduct(ncohesion_delta, boid->direction);
  float cohesion_dlength = Vector2Length(cohesion_delta);

  boid->acceleration = Lerp(boid->acceleration,
                            cohesion_dot * (cohesion_dlength / BOID_VIEWDIST) *
                                BOID_COHESION_SPEED_FACTOR,
                            BOID_ACCELERATION);

  float speed = BOID_MOVE_SPEED + boid->acceleration;

  // rest of calcus
  boid->pos->x += boid->direction.x * speed;
  boid->pos->y += boid->direction.y * speed;

  boid->sprite.rotation = Vector2Angle(v2up, boid->direction) * RAD2DEG;

  SpriteDraw(&boid->sprite);

  // screen bounds teleport
  int sw = GetScreenWidth();
  int sh = GetScreenHeight();
  if (boid->pos->x > sw) {
    boid->pos->x = 0;
  } else if (boid->pos->x < 0) {
    boid->pos->x = sw;
  }
  if (boid->pos->y > sh) {
    boid->pos->y = 0;
  } else if (boid->pos->y < 0) {
    boid->pos->y = sh;
  }
}

static DemoBoid0 *add_boid(DemoBoids0State *state) {
  Memspace *memspace = state->memspaces.first->point;
  Memcell *memcell = MemcellAllocate(&state->memblock);
  DemoBoid0 *boid = memcell->point;
  boid->pos = &boid->sprite.position;
  MemspaceAssign(memspace, memcell, (Tynvec2 *)&boid->sprite.position);

  SpriteInit(&boid->sprite, state->texture_boid);
  boid->sprite.scale = 1;

  boid->pos->x = 256 + GetRandomValue(-256, 256);
  boid->pos->y = 256 + GetRandomValue(-256, 256);
  Vector2 dir = (Vector2){GetRandomValue(-256, 256), GetRandomValue(-256, 256)};
  boid->direction = Vector2Normalize(dir);
  boid->torque = 0;
  boid->acceleration = 0;
	boid->energy = 1;

  return boid;
}

static void _init_boids(DemoBoids0State *state) {
  for (int i = 0; i < BOIDS_AMOUNT; i++) {
    add_boid(state);
  }
}

static void _init(DemoBoids0State *state) {
  Memblock *boids = MemblockInit(&state->memblock, sizeof(DemoBoid0));
  Memblock *memspaces = MemblockInit(&state->memspaces, sizeof(Memspace));
  MemspaceAllocate(memspaces);
  state->texture_boid = LoadTexture("res/ship_A.png");
  _init_boids(state);
}

DemoBoids0State *DemoBoids0Init(TynStage *stage) {
  DemoBoids0State *state = malloc(sizeof(DemoBoids0State));
  if (state == NULL) {
    return NULL;
  }

  stage->state = state;
  stage->frame = (TynFrame){&_dispose, &_step, &_draw, &_cmdin, &_cmdout};

  _init(state);

  return stage->state;
}

static void _dispose(DemoBoids0State *state) {
  MemblockDispose(&state->memblock);
  MemspaceDispose(&state->memspaces);
  MemblockDispose(&state->memspaces);
  free(state);
}

static STAGEFLAG _step(DemoBoids0State *state, STAGEFLAG flags) {
  return flags;
}

static void _draw(DemoBoids0State *state) {
  ClearBackground(BLACK);
  MemspaceUpdate(&state->memspaces, draw_boid);

  if (BOID_DEBUG) {
    int index = 0;
    for (Memcell *m = state->memspaces.first; m; m = m->next) {
      const Memspace *memspace = m->point;
      const Tynvec2 min = memspace->bounds.min;
      const Tynvec2 max = memspace->bounds.max;
      if (memspace->depth == 0) {
        DrawRectangleLines(min.x, min.y, max.x - min.x, max.y - min.y, RED);
      } else if (memspace->depth % 2 == 0) {
        DrawLine(max.x, min.y, max.x, max.y, BLUE);
      } else if (memspace->depth % 2 == 1) {
        DrawLine(min.x, max.y, max.x, max.y, BLUE);
      }
    }
  }
}

static char *_cmdin(DemoBoids0State *state, STAGEFLAG *flags) { return NULL; }

static char *_cmdout(DemoBoids0State *state, char *message) {
  if (strcmp(message, "??") == 0) {
    return "commands:\n"
           "al=x: set alignment \n"
           "ch=x: set cohesion\n"
           "sp=x: set separation\n";
  }

  if (strcmp(message, "dbg") == 0) {
    BOID_DEBUG = !BOID_DEBUG;
  } else {
    char arg[4] = {0};
    float val = 0;
    if (sscanf(message, "%3s=%f", arg, &val) == 2) {
			TraceLog(LOG_INFO, arg);
      if (strcmp(arg, "alg") == 0) {
        BOID_ALIGMENT_FACTOR = val;
      } else if (strcmp(arg, "coh") == 0) {
        BOID_COHESION_FACTOR = val;
      } else if (strcmp(arg, "sep") == 0) {
        BOID_SEPARATION_FACTOR = val;
      } else if (strcmp(arg, "spd") == 0) {
        BOID_MOVE_SPEED = val;
      } else if (strcmp(arg, "cen") == 0) {
        BOID_CENTRIC_FACTOR = val;
      }

    }
  }

  return message;
}
