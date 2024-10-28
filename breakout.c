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
	bool loss;
	bool win;
	bool paused;
} State;
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
	Vector2 direction;

	float radius;
	Color color;
} Ball;

int main(void)
{
	char *title = "BREAKOUT";
	char *level_win = "You Won!";
	char *level_loss = "You Lost!";
	char *paused = "Press Space to continue";
	int title_font_size = 30;
	int big_font_size = 40;
	bool debug = false;
	float screen_width = 1920.0f;
	float screen_height = 1080.0f;
	float num_row = 10.0;

	bool paddle_collision = false;
	bool block_collision = false;

	State state = { .win = false, .loss = false, .paused = true };

	float block_width = screen_width / num_row;

	Block blocks[NUM_ROW] = { 0 };
	float rec_width = screen_width / 5.0;
	Line ruler[RULER_INC] = { 0 };

	Ball ball = {
		.centre.x = screen_width / 2.0,
		.centre.y = screen_height - (screen_height / 4),
		.velocity.x = 2.0,
		.velocity.y = 4.0,
		.direction.x = 1,
		.direction.y = 1,
		.radius = 20.0,
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
	for (int i = block_width; i < screen_width - block_width;
	     i += block_width) {
		blocks[block_count] = (Block){
			.rec = (Rectangle){ .x = i,
					    .y = screen_height / 3.0 - 10.0,
					    .width = block_width,
					    .height = block_width / 2 },
			.alive = true,
		};

		block_count++;
	}

	int alive_blocks = block_count;
	int end_counter = 1200;

	SetTargetFPS(144);

	InitWindow((int)screen_width, (int)screen_height,
		   "raylib [core] example - basic window");

	while (!WindowShouldClose()) {
		// update
		// ------------------------------------------------
		if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
			paddle.x += 10.0f;
		}
		if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
			paddle.x -= 10.0f;
		}

		if (IsKeyDown(KEY_SPACE)) {
			state.paused = !state.paused;
		}

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
				alive_blocks--;
				if (alive_blocks == 0) {
					state.win = true;
				}
				ball.direction.y *= -1.0;
				ball.direction.x *= 1.0;
				break;
			}
		}

		if (paddle_collision) {
			ball.direction.y *= -1.0;
		}

		if ((ball.centre.x >= (screen_width - ball.radius)) ||
		    (ball.centre.x <= ball.radius)) {
			ball.direction.x *= -1.0f;
		}

		if (ball.centre.y >= (screen_height - ball.radius)) {
			state.loss = true;
		}
		if ((ball.centre.y <= ball.radius)) {
			ball.direction.y *= -1.0f;
		}

		if (end_counter != 0) {
			if (state.win) {
				ball.velocity.x -= 0.01f;
				ball.velocity.y -= 0.01f;
				end_counter--;
			}
		} else {
			ball.velocity.x = 0.0f;
			ball.velocity.y = 0.0f;
		}

		if (!state.paused) {
			ball.centre.x += (ball.velocity.x * ball.direction.x);
			ball.centre.y += (ball.velocity.y * ball.direction.y);
		}

		// ----------------------------------------------
		BeginDrawing();
		ClearBackground(BLACK);

		DrawFPS(10, 10);
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

		if (state.paused) {
			DrawText(paused,
				 screen_width / 2 -
					 ((float)MeasureText(paused,
							     big_font_size) /
					  2),
				 screen_height / 2, big_font_size, LIGHTGRAY);

			DrawText(title,
				 screen_width / 2 -
					 ((float)MeasureText(title,
							     title_font_size) /
					  2),
				 screen_height / 7, title_font_size, GREEN);
		}
		if (state.win) {
			DrawText(level_win,
				 screen_width / 2 -
					 ((float)MeasureText(level_win,
							     big_font_size) /
					  2),
				 screen_height / 2, big_font_size, YELLOW);
		}

		if (state.loss && !state.win) {
			DrawText(level_loss,
				 screen_width / 2 -
					 ((float)MeasureText(level_loss,
							     big_font_size) /
					  2),
				 screen_height / 2, big_font_size, ORANGE);
		}
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
