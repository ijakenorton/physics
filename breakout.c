#include "raylib.h"
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM_ROW 20
#define RULER_INC 96
#define MAX_LEVEL 2
#define expand_vec2(vec) vec.x, vec.y
#define expand_rec(rec) rec.x, rec.y, rec.width, rec.height
#define print_int(val) printf(#val " = %d\n", val)
#define print_rec(rec) \
	printf("x = %f\ny = %f\nwidth = %f\nheight = %f\n", expand_rec(rec))
#define print_ball(ball)                                                                                                            \
	printf("centre.x = %f\ncentre.y = %f\nvelocity.x = %f\nvelocity.y = %f\ndirection.x = %f\ndirection.y = %f\nradius = %f\n", \
	       expand_vec2(ball->centre), expand_vec2(ball->velocity),                                                              \
	       expand_vec2(ball->direction), ball->radius)

#define print_block(block)                                           \
	printf("x = %f y = %f\nwidth = %f height = %f alive = %d\n", \
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
	Block *blocks;
	int num_alive;
	int num_blocks;
} BlocksState;

typedef struct {
	Vector2 centre;
	Vector2 velocity;
	Vector2 direction;

	float radius;
	Color color;
} Ball;

typedef struct {
	BlocksState *blocks_state;
	Ball *ball;
	Rectangle *paddle;
} Level;

typedef struct {
	bool loss;
	bool win;
	bool paused;
	Level *cl;
	int level_no;
} State;

typedef void (*LevelFunc)(Level *level, State *state);

float screen_width = 1920.0f / 2;
float screen_height = 1080.0f / 2;

void assert_with_message(bool condition, char *message)
{
	print_int(condition);
	if (!condition) {
		fprintf(stderr, "%s", message);
		exit(1);
	}
}

void init_level_two(Level *level_two, State *state)
{
	if (level_two->ball == NULL) {
		level_two->ball = malloc(sizeof(Ball));
	}

	*level_two->ball = (Ball){
		.centre.x = screen_width / 2.0,
		.centre.y = screen_height - (screen_height / 4),
		.velocity.x = 2.0,
		.velocity.y = 4.0,
		.direction.x = 1,
		.direction.y = 1,
		.radius = 20.0,
		.color = BLUE,
	};

	if (level_two->paddle == NULL) {
		level_two->paddle = malloc(sizeof(Rectangle));
	}

	float rec_width = screen_width / 5.0;
	*level_two->paddle =
		(Rectangle){ .x = screen_width / 2.0 - (rec_width / 2),
			     .y = screen_height - 50.0,
			     .width = rec_width,
			     .height = 20.0 };

	float num_row = 10.0;
	float block_width = screen_width / num_row;
	float block_height = block_width / 2;

	if (level_two->blocks_state == NULL) {
		level_two->blocks_state = malloc(sizeof(BlocksState));
		level_two->blocks_state->blocks =
			malloc(sizeof(Block) * NUM_ROW);
	}

	// init blocks
	int block_count = 0;
	for (int i = block_width; i < screen_width - block_width;
	     i += block_width) {
		level_two->blocks_state->blocks[block_count] = (Block){
			.rec = (Rectangle){ .x = i,
					    .y = screen_height / 3.0,
					    .width = block_width,
					    .height = block_height },
			.alive = true,
		};

		block_count++;
	}

	for (int i = block_width + block_height;
	     i < screen_width - block_width * 2; i += block_width) {
		level_two->blocks_state->blocks[block_count] = (Block){
			.rec = (Rectangle){ .x = i,
					    .y = screen_height / 3.0 +
						 block_height,
					    .width = block_width,
					    .height = block_height },
			.alive = true,
		};

		block_count++;
	}
	level_two->blocks_state->num_alive = block_count;
	level_two->blocks_state->num_blocks = block_count;

	*state = (State){ .win = false,
			  .loss = false,
			  .paused = true,
			  .cl = level_two,
			  .level_no = 1 };
}

// TODO cleanup memory
// Redundent passing the level and state as I'm currently using it but unsure if thats correct
// So leaving the option
void init_level_one(Level *level_one, State *state)
{
	if (level_one->ball == NULL) {
		level_one->ball = malloc(sizeof(Ball));
	}

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

	if (level_one->paddle == NULL) {
		level_one->paddle = malloc(sizeof(Rectangle));
	}

	float rec_width = screen_width / 5.0;
	*level_one->paddle =
		(Rectangle){ .x = screen_width / 2.0 - (rec_width / 2),
			     .y = screen_height - 50.0,
			     .width = rec_width,
			     .height = 20.0 };

	float num_row = 10.0;
	float block_width = screen_width / num_row;

	if (level_one->blocks_state == NULL) {
		level_one->blocks_state = malloc(sizeof(BlocksState));
		level_one->blocks_state->blocks =
			malloc(sizeof(Block) * NUM_ROW);
	}

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
	level_one->blocks_state->num_blocks = block_count;

	*state = (State){ .win = false,
			  .loss = false,
			  .paused = true,
			  .cl = level_one,
			  .level_no = 0 };
}

int main(void)
{
	char *title = "BREAKOUT";
	char *reset = "Reset: R";
	char *level_win = "You Won!";
	char *level_loss = "You Lost!";
	char *paused = "Press Space to continue";
	int title_font_size = 30;
	int big_font_size = 40;
	bool debug = false;

	bool paddle_collision = false;
	bool block_collision = false;

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

	State state = { 0 };
	LevelFunc levels[2] = { 0 };

	state.cl = malloc(sizeof(Level));
	state.cl->blocks_state = NULL;
	state.cl->ball = NULL;
	state.cl->paddle = NULL;
	levels[0] = init_level_one;
	levels[1] = init_level_two;
	levels[state.level_no](state.cl, &state);

	int end_counter = 1200;

	SetTargetFPS(144);

	InitWindow((int)screen_width, (int)screen_height,
		   "raylib [core] example - basic window");

	int paused_cooldown = 1;
	int initial_timer = 1;
	while (!WindowShouldClose()) {
		// update
		// ------------------------------------------------
		if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
			state.cl->paddle->x += 10.0f;
		}
		if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
			state.cl->paddle->x -= 10.0f;
		}

		// Bit of a hack to fix space being triggered multiple times
		// Handles pause
		if (IsKeyReleased(KEY_SPACE)) {
			if (paused_cooldown == initial_timer) {
				state.paused = !state.paused;
			} else {
				if (paused_cooldown == 0) {
					paused_cooldown = initial_timer;
				} else {
					paused_cooldown--;
				}
			}
		}

		if (IsKeyReleased(KEY_R)) {
			levels[state.level_no](state.cl, &state);
		}

		if (IsKeyReleased(KEY_ENTER)) {
			//currently levels can be changed without winning
			//as a dev door to help testing

			/* if (state.win) { */
			state.level_no++;
			char state_assert_msg[100] = { 0 };

			sprintf(state_assert_msg,
				"State.level_no is: %d, must be less than %d\n",
				state.level_no, MAX_LEVEL);

			assert_with_message(state.level_no < MAX_LEVEL,
					    state_assert_msg);

			levels[state.level_no](state.cl, &state);
			/* } */
		}

		paddle_collision = CheckCollisionCircleRec(
			state.cl->ball->centre, state.cl->ball->radius,
			*state.cl->paddle);

		for (int i = 0; i < state.cl->blocks_state->num_blocks; ++i) {
			Block current_block = state.cl->blocks_state->blocks[i];

			if (!state.cl->blocks_state->blocks[i].alive) {
				continue;
			}

			block_collision = CheckCollisionCircleRec(
				state.cl->ball->centre, state.cl->ball->radius,
				current_block.rec);

			if (block_collision) {
				state.cl->blocks_state->blocks[i].alive = false;
				state.cl->blocks_state->num_alive--;
				if (state.cl->blocks_state->num_alive == 0) {
					state.win = true;
				}
				state.cl->ball->direction.y *= -1.0;
				state.cl->ball->direction.x *= 1.0;

				break;
			}
		}

		if (paddle_collision) {
			state.cl->ball->direction.y *= -1.0;
		}

		if ((state.cl->ball->centre.x >=
		     (screen_width - state.cl->ball->radius)) ||
		    (state.cl->ball->centre.x <= state.cl->ball->radius)) {
			state.cl->ball->direction.x *= -1.0f;
		}

		if (state.cl->ball->centre.y >=
		    (screen_height - state.cl->ball->radius)) {
			state.loss = true;
		}
		if ((state.cl->ball->centre.y <= state.cl->ball->radius)) {
			state.cl->ball->direction.y *= -1.0f;
		}

		if (end_counter != 0) {
			if (state.win) {
				state.cl->ball->velocity.x -= 0.01f;
				state.cl->ball->velocity.y -= 0.01f;
				end_counter--;
			}
		} else {
			state.cl->ball->velocity.x = 0.0f;
			state.cl->ball->velocity.y = 0.0f;
		}

		if (!state.paused) {
			state.cl->ball->centre.x +=
				(state.cl->ball->velocity.x *
				 state.cl->ball->direction.x);
			state.cl->ball->centre.y +=
				(state.cl->ball->velocity.y *
				 state.cl->ball->direction.y);
		}

		// ----------------------------------------------
		BeginDrawing();
		ClearBackground(BLACK);

		DrawFPS(10, 10);
		for (int row = 0; row < state.cl->blocks_state->num_blocks;
		     ++row) {
			Block current_block =
				state.cl->blocks_state->blocks[row];

			if (current_block.alive) {
				DrawRectangleRec(current_block.rec, RED);
				DrawRectangleLinesRec(current_block.rec, WHITE);
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

		DrawRectangleRec(*state.cl->paddle, WHITE);
		DrawCircle(state.cl->ball->centre.x, state.cl->ball->centre.y,
			   state.cl->ball->radius, state.cl->ball->color);

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

			DrawText(reset,
				 screen_width - screen_width / 10 -
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
