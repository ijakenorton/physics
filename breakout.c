#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#define NUM_ROW 20
#define RULER_INC 96
#define print_int(val) printf(#val " = %d\n", val)
#define expand_rec(rec) rec.x, rec.y, rec.width, rec.height
#define print_rec(rec) \
	printf("x = %f\ny = %f\nwidth = %f\nheight = %f\n", expand_rec(rec))

#define print_block(block)                                              \
	printf("x = %f\ny = %f\nwidth = %f\nheight = %f\nalive = %d\n", \
	       expand_rec(block.rec), block.alive)

#define DrawRectangleLinesRec(rec, colour) \
	DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, colour)

typedef struct {
	int startPosX;
	int startPosY;
	int endPosX;
	int endPosY;
	Color color;
} Line;

typedef struct {
	Rectangle rec;
	bool alive;
} Block;

typedef struct {
	Vector2 centre;
	Vector2 velocity;

	float radius;
	Color color;
} Ball;

int main(void)
{
	bool debug = true;
	float screen_width = 960.0f;
	float screen_height = 540.0f;
	float num_row = 20.0;

	float block_dimension = screen_width / num_row;

	Block blocks[NUM_ROW] = { 0 };
	float rec_width = screen_width / 5.0;
	Line ruler[RULER_INC] = { 0 };

	Ball ball = {
		.centre.x = screen_width / 2.0,
		.centre.y = screen_height - (screen_height / 4),
		.velocity.x = 0.5,
		.velocity.y = 1.0,
		.radius = 10.0,
		.color = BLUE,
	};

	Rectangle paddle =
		(Rectangle){ .x = screen_width / 2.0 - (rec_width / 2),
			     .y = screen_height - 50.0,
			     .width = rec_width,
			     .height = 20.0 };

	Line horizontal = (Line){
		.startPosX = 0,
		.startPosY = screen_height / 2.0,
		.endPosX = screen_width,
		.endPosY = screen_height / 2.0,
		.color = WHITE,
	};

	Line vertical = (Line){
		.startPosX = screen_width / 2.0,
		.startPosY = screen_height,
		.endPosX = screen_width / 2.0,
		.endPosY = 0,
		.color = WHITE,
	};
	int ruler_count = 0;

	// init ruler
	for (int i = 0; i <= screen_width + 1; i += screen_width / 10) {
		ruler[ruler_count] = (Line){
			.startPosX = i,
			.startPosY = screen_height,
			.endPosX = i,
			.endPosY = screen_height - 20,
			.color = WHITE,
		};
		ruler_count++;
	}
	// init blocks
	int block_count = 0;
	for (int i = block_dimension; i < screen_width - block_dimension;
	     i += block_dimension) {
		blocks[block_count] = (Block){
			.rec = (Rectangle){ .x = i,
					    .y = screen_height / 3.0 - 10.0,
					    .width = block_dimension,
					    .height = block_dimension },
			.alive = true,
		};

		block_count++;
	}

	bool paddle_collision = false;
	bool block_collision = false;

	SetTargetFPS(144);

	InitWindow((int)screen_width, (int)screen_height,
		   "raylib [core] example - basic window");

	while (!WindowShouldClose()) {
		paddle_collision = CheckCollisionCircleRec(ball.centre,
							   ball.radius, paddle);

		for (int i = 0; i < block_count; ++i) {
			if (!blocks[i].alive) {
				continue;
			}

			block_collision = CheckCollisionCircleRec(
				ball.centre, ball.radius, blocks[i].rec);

			if (block_collision) {
				blocks[i].alive = false;
				ball.velocity.y *= -1.1;
				ball.velocity.x *= 1.1;
				break;
			}
		}

		if (paddle_collision) {
			ball.velocity.y *= -1.0;
		}

		if ((ball.centre.x >= (screen_width - ball.radius)) ||
		    (ball.centre.x <= ball.radius)) {
			ball.velocity.x *= -1.0f;
		}

		if ((ball.centre.y >= (screen_height - ball.radius)) ||
		    (ball.centre.y <= ball.radius)) {
			ball.velocity.y *= -1.0f;
		}

		ball.centre.x += ball.velocity.x;
		ball.centre.y += ball.velocity.y;

		BeginDrawing();
		ClearBackground(BLACK);

		for (int row = 0; row < block_count; ++row) {
			if (blocks[row].alive) {
				DrawRectangleRec(blocks[row].rec, RED);
				DrawRectangleLinesRec(blocks[row].rec, WHITE);
			}
		}

		if (debug) {
			for (int line = 0; line < ruler_count; ++line) {
				DrawLine(ruler[line].startPosX,
					 ruler[line].startPosY,
					 ruler[line].endPosX,
					 ruler[line].endPosY,
					 ruler[line].color);
			}

			DrawLine(vertical.startPosX, vertical.startPosY,
				 vertical.endPosX, vertical.endPosY,
				 vertical.color);

			DrawLine(horizontal.startPosX, horizontal.startPosY,
				 horizontal.endPosX, horizontal.endPosY,
				 horizontal.color);
		}

		DrawRectangleRec(paddle, WHITE);
		DrawCircle(ball.centre.x, ball.centre.y, ball.radius,
			   ball.color);

		DrawText("BREAKOUT", screen_width / 2 - screen_width / 18,
			 screen_height / 7, 20, LIGHTGRAY);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
