#include "./include/tests.h"
#include "raylib.h"
#include "rlgl.h"
#include "math.h"

void _draw_test_origami() {
	float t = GetTime();
  Camera3D camera = {0};
  camera.position = (Vector3){0.0f, 3.0f, 0.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 0.0f, -1.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

	rlDisableBackfaceCulling();
  BeginMode3D(camera);

	Vector3 pos = { 0, 0, 0 };
	Vector2 size = { 1, 1 };

	rlPushMatrix();
		rlTranslatef(pos.x, pos.y, pos.z);
		rlScalef(size.x, 1.0f, size.y);

		rlBegin(RL_TRIANGLES);
			rlColor4ub(WHITE.r, WHITE.g, WHITE.b, WHITE.a);
			rlNormal3f(0.0f, 1.0f, 0.0f);

			rlVertex3f(-0.5f, 0.0f, -0.5f);
			rlVertex3f(-0.5f, 0.0f, 0.5f);
			rlVertex3f(0.5f, 0.0f, 0.5f);

			float s = sinf(t) * 0.5;
			float c = cosf(t) * 0.5;

			rlColor4ub(RED.r, RED.g, RED.b, RED.a);
			rlVertex3f(-0.5f, 0.0f, -0.5f);
			rlVertex3f(c, fabsf(s), -c);
			rlVertex3f(0.5f, 0.0f, 0.5f);
		rlEnd();
	rlPopMatrix();

  EndMode3D();
	rlEnableBackfaceCulling();
}
