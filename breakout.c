#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
	Block *blocks;
	int num_alive;
} Blocks;

typedef struct {
	Vector2 centre;
	Vector2 velocity;
	Vector2 direction;

	float radius;
	Color color;
} Ball;

typedef struct {
	Blocks *blocks_state;
	Ball *ball;
	Rectangle *paddle;
} LevelOne;

float screen_width = 1920.0f;
float screen_height = 1080.0f;

// TODO Cleanup memory from init
void init_level_one(LevelOne *level_one)
{
	level_one->ball = malloc(sizeof(Ball));

	*level_one->ball = (Ball){
		.centre.x = screen_width / 2.0,
		.centre.y = screen_height - (screen_height / 4),
		.velocity.x = 2.0,
		.velocity.y = 4.0,
		.direction.x = 1,
		.direction.y = 1,
		.radius = 20.0,
		.color = BLUE,
	};

	level_one->paddle = malloc(sizeof(Rectangle));

	float rec_width = screen_width / 5.0;
	*level_one->paddle =
		(Rectangle){ .x = screen_width / 2.0 - (rec_width / 2),
			     .y = screen_height - 50.0,
			     .width = rec_width,
			     .height = 20.0 };

	float num_row = 10.0;
	float block_width = screen_width / num_row;

	level_one->blocks_state = malloc(sizeof(Blocks));
	level_one->blocks_state->blocks = malloc(sizeof(Block) * NUM_ROW);

	// TODO Something wrong with init or collision
	// init blocks
	int block_count = 0;
	for (int i = block_width; i < screen_width - block_width;
	     i += block_width) {
		level_one->blocks_state->blocks[block_count] = (Block){
			.rec = (Rectangle){ .x = i,
					    .y = screen_height / 3.0 - 10.0,
					    .width = block_width,
					    .height = block_width / 2 },
			.alive = true,
		};

		block_count++;
	}
	level_one->blocks_state->num_alive = block_count;
}

int main(void)
{
	char *title = "BREAKOUT";
	char *level_win = "You Won!";
	char *level_loss = "You Lost!";
	char *paused = "Press Space to continue";
	int title_font_size = 30;
	int big_font_size = 40;
	bool debug = false;

	bool paddle_collision = false;
	bool block_collision = false;

	State state = { .win = false, .loss = false, .paused = true };

	Line ruler[RULER_INC] = { 0 };

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

	LevelOne l1 = { 0 };
	init_level_one(&l1);
	print_block(l1.blocks_state->blocks[0]);
	int end_counter = 1200;

	SetTargetFPS(144);

	InitWindow((int)screen_width, (int)screen_height,
		   "raylib [core] example - basic window");

	while (!WindowShouldClose()) {
		// update
		// ------------------------------------------------
		if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
			l1.paddle->x += 10.0f;
		}
		if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
			l1.paddle->x -= 10.0f;
		}

		if (IsKeyDown(KEY_SPACE)) {
			state.paused = !state.paused;
		}

		/* if (IsKeyDown(KEY_R)) { */
		/* 	state.paused = !state.paused; */
		/* } */

		// TODO Something wrong with init or collision
		paddle_collision = CheckCollisionCircleRec(
			l1.ball->centre, l1.ball->radius, *l1.paddle);

		for (int i = 0; i < l1.blocks_state->num_alive; ++i) {
			if (!l1.blocks_state->blocks[i].alive) {
				continue;
			}

			block_collision = CheckCollisionCircleRec(
				l1.ball->centre, l1.ball->radius,
				l1.blocks_state->blocks[i].rec);

			if (block_collision) {
				l1.blocks_state->blocks[i].alive = false;
				l1.blocks_state->num_alive--;
				if (l1.blocks_state->num_alive == 0) {
					state.win = true;
				}
				l1.ball->direction.y *= -1.0;
				l1.ball->direction.x *= 1.0;
				break;
			}
		}

		if (paddle_collision) {
			l1.ball->direction.y *= -1.0;
		}

		if ((l1.ball->centre.x >= (screen_width - l1.ball->radius)) ||
		    (l1.ball->centre.x <= l1.ball->radius)) {
			l1.ball->direction.x *= -1.0f;
		}

		if (l1.ball->centre.y >= (screen_height - l1.ball->radius)) {
			state.loss = true;
		}
		if ((l1.ball->centre.y <= l1.ball->radius)) {
			l1.ball->direction.y *= -1.0f;
		}

		if (end_counter != 0) {
			if (state.win) {
				l1.ball->velocity.x -= 0.01f;
				l1.ball->velocity.y -= 0.01f;
				end_counter--;
			}
		} else {
			l1.ball->velocity.x = 0.0f;
			l1.ball->velocity.y = 0.0f;
		}

		if (!state.paused) {
			l1.ball->centre.x +=
				(l1.ball->velocity.x * l1.ball->direction.x);
			l1.ball->centre.y +=
				(l1.ball->velocity.y * l1.ball->direction.y);
		}

		// ----------------------------------------------
		BeginDrawing();
		ClearBackground(BLACK);

		DrawFPS(10, 10);
		for (int row = 0; row < l1.blocks_state->num_alive; ++row) {
			if (l1.blocks_state->blocks[row].alive) {
				DrawRectangleRec(
					l1.blocks_state->blocks[row].rec, RED);
				DrawRectangleLinesRec(
					l1.blocks_state->blocks[row].rec,
					WHITE);
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

		DrawRectangleRec(*l1.paddle, WHITE);
		DrawCircle(l1.ball->centre.x, l1.ball->centre.y,
			   l1.ball->radius, l1.ball->color);

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
