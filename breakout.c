#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#define NUM_ROW 20
#define RULER_INC 96
#define print_int(val) printf(#val " = %d\n", val)

#define print_rec(rec)                                                    \
	printf("x = %f\ny = %f\nwidth = %f\nheight = %f\n", rec.x, rec.y, \
	       rec.width, rec.height)

typedef struct {
	int startPosX;
	int startPosY;
	int endPosX;
	int endPosY;
	Color color;
} Line;

typedef struct {
	int centerX;
	int centerY;
	float radius;
	Color color;
} Circle;

int main(void)
{
	bool debug = true;
	float screen_width = (960);
	float screen_height = (540);
	float num_row = 20.0;

	InitWindow((int)screen_width, (int)screen_height,
		   "raylib [core] example - basic window");
	float block_dimension = screen_width / num_row;

	Rectangle blocks[NUM_ROW] = { 0 };
	float rec_width = screen_width / 5.0;
	Line ruler[RULER_INC] = { 0 };

	Circle ball = {
		.centerX = screen_width / 2.0,
		.centerY = screen_height - (screen_height / 4),
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
	int count = 0;
	for (int i = block_dimension; i < screen_width - block_dimension;
	     i += block_dimension) {
		blocks[count] = (Rectangle){ .x = i,
					     .y = screen_height / 3.0 - 10.0,
					     .width = block_dimension,
					     .height = block_dimension };
		count++;
	}

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		for (int row = 0; row < count; ++row) {
			DrawRectangle(blocks[row].x, blocks[row].y,
				      blocks[row].width, blocks[row].height,
				      RED);
			DrawRectangleLines(blocks[row].x, blocks[row].y,
					   blocks[row].width,
					   blocks[row].height, WHITE);
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

		DrawRectangle(paddle.x, paddle.y, paddle.width, paddle.height,
			      WHITE);
		DrawCircle(ball.centerX, ball.centerY, ball.radius, ball.color);

		DrawText("BREAKOUT", screen_width / 2 - screen_width / 18,
			 screen_height / 7, 20, LIGHTGRAY);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
