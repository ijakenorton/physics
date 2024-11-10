#include "raylib.h"
#include "types.h"
#include "collision.h"
#include <stdio.h>
#include <stdlib.h>
#define ARENA_IMPLEMENTATION
#include "arena.h"

float screen_width = 1080.0f ;
float screen_height = 1080.0f;

static Arena default_arena = { 0 };
/* static Arena temporary_arena = { 0 }; */
static Arena *context_arena = &default_arena;

void *context_alloc(size_t size)
{
	assert(context_arena);
	return arena_alloc(context_arena, size);
}

void assert_with_message(bool condition, char *message)
{
	print_int(condition);
	if (!condition) {
		fprintf(stderr, "%s", message);
		exit(1);
	}
}

Vector2 vec_add(Vector2 v1, Vector2 v2)
{
        return (Vector2){.x = v1.x + v2.x, .y = v1.y + v2.y};
}


void world_to_screen(Vector2 *new_v, Vector2 *old_v)
{
        new_v->x = old_v->x - screen_width / 2.0f; 
        new_v->y = old_v->y - screen_height/ 2.0f;
}

void screen_to_world(Vector2 *new_v, Vector2 *old_v)
{
        new_v->x = old_v->x + screen_width / 2.0f; 
        new_v->y = old_v->y + screen_height/ 2.0f;
}


void line_world_to_screen(Line *new_l,Line *old_l)
{
        world_to_screen(&new_l->start,&old_l->start);
        world_to_screen(&new_l->end,&old_l->end);
        new_l->color = old_l->color;
}

void line_screen_to_world(Line *new_l,Line *old_l)
{
        screen_to_world(&new_l->start,&old_l->start);
        screen_to_world(&new_l->end,&old_l->end);
        new_l->color = old_l->color;
}

void vec2_linear_translate(Vector2 *new_v, Vector2 *old_v, Translation_Matrix m)
{
        Vector2 temp = {old_v->x, old_v->y};
        print_float_v2(temp);
        print_float(m.x1);
        print_float(m.x2);
        print_float(m.y1);
        print_float(m.y2);

        print_float(m.y2);
        print_float(m.y2);
        print_float(m.y2);
        print_float(m.y2);
        new_v->x = (m.x1 * old_v->x) + (m.x2 * old_v->x);
        new_v->y = (m.y1 * old_v->y) + (m.y2 * old_v->y);

        temp.x = new_v->x;
        temp.y = new_v->y;
        print_float_v2(temp);
}

int main(void)
{
	bool debug = true;

	Line horizontal = (Line){
		.start.x = 0,
		.start.y = screen_height / 2.0,
		.end.x   = screen_width,
		.end.y   = screen_height / 2.0,
		.color   = WHITE,
	};

	Line vertical = (Line){
		.start.x = screen_width / 2.0,
		.start.y = screen_height,
		.end.x   = screen_width / 2.0,
		.end.y   = 0,
		.color   = WHITE,
	};

	int x_count = 0;
	int y_count = 0;

	// init ruler

	Line y_lines[RULER_INC] = { 0 };
	for (float i = 0.0f; i <= screen_width + 1; i += screen_width / 10.0f) {
		y_lines[y_count] = (Line){
		.start.x = i,
		.start.y = screen_height,
		.end.x   = i,
		.end.y   = 0.0f,
		.color   = GREEN,
		};

		y_count++;
	}

	Line x_lines[RULER_INC] = { 0 };
	for (float i = 0.0f; i <= screen_height + 1; i += screen_height / 10.0f) {
		x_lines[x_count] = (Line){
			.start.x = screen_width,
			.start.y = i,
			.end.x   = 0.0f,
			.end.y   = i,
			.color   = WHITE,
		};
		x_count++;
	}


// translated
	Line screen_y_lines[RULER_INC] = { 0 };
	for (int i = 0; i < y_count; ++i ) {
		line_world_to_screen(&screen_y_lines[i] ,&y_lines[i]);
	}

	Line screen_x_lines[RULER_INC] = { 0 };
	for (int i = 0; i < x_count; ++i ) {
		line_world_to_screen(&screen_x_lines[i] ,&x_lines[i]);
                }


// translated back
	Line world_y_lines[RULER_INC] = { 0 };
	for (int i = 0; i < y_count; ++i ) {
		line_screen_to_world(&world_y_lines[i] ,&screen_y_lines[i]);
	}

	Line world_x_lines[RULER_INC] = { 0 };
	for (int i = 0; i < x_count; ++i ) {
		line_screen_to_world(&world_x_lines[i] ,&screen_x_lines[i]);
	}



        Rectangle origin_rec = {
            .x = 0,                // Rectangle top-left corner position x
            .y = 0,                // Rectangle top-left corner position y
            .width = screen_width /10.0f,            // Rectangle width
            .height = screen_height / 10.0f,           // Rectangle height
        };

        Vector2 translated_origin = { 0 };

        screen_to_world(&translated_origin,&(Vector2){origin_rec.x, origin_rec.y,});

        Rectangle translated_rec =  {
                .x = translated_origin.x,
                .y = translated_origin.y,
                .width = screen_width /10.0f,           
                .height = screen_height / 10.0f,        
        };
        print_rec(translated_rec);

        Vector2 rotated_origin = { 0 };

        Line origin_line = (Line){
			.start.x = translated_origin.x,
			.start.y = translated_origin.y,
			.end.x   = translated_origin.y,
			.end.y   = translated_origin.x + screen_width / 5.0f,
			.color   = YELLOW,
		};

        vec2_linear_translate(&rotated_origin,&(Vector2){origin_line.end.x, origin_line.end.y,}, 
                              (Translation_Matrix) {.x1 =0.0f, .y1 = 1.0f, .x2 = -1.0f, .y2 = 0}); 


        Line rotated_line = (Line){
			.start.x = translated_origin.x,
			.start.y = translated_origin.y,
			.end.x   = rotated_origin.x,
			.end.y   = rotated_origin.y,
			.color   = BLUE,
		};


	InitWindow((int)screen_width, (int)screen_height, "Physics stuff");

	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(BLACK);
		// update
		// ------------------------------------------------
		// Bit of a hack to fix space being triggered multiple times
		// Handles pause

		if (debug) {

			for (int line = 0; line < y_count; ++line) {
				DrawLine(y_lines[line].start.x,
					 y_lines[line].start.y,
					 y_lines[line].end.x  ,
					 y_lines[line].end.y  ,
					 y_lines[line].color);
			}

			for (int line = 0; line < x_count; ++line) {
				DrawLine(x_lines[line].start.x,
					 x_lines[line].start.y,
					 x_lines[line].end.x  ,
					 x_lines[line].end.y  ,
					 x_lines[line].color);
			}

                        /* // translate to world from screen */
			for (int line = 0; line < y_count; ++line) {
				DrawLine(world_y_lines[line].start.x,
					 world_y_lines[line].start.y,
					 world_y_lines[line].end.x  ,
					 world_y_lines[line].end.y  ,
					 VIOLET);
			}

			for (int line = 0; line < x_count; ++line) {
				DrawLine(world_x_lines[line].start.x,
					 world_x_lines[line].start.y,
					 world_x_lines[line].end.x  ,
					 world_x_lines[line].end.y  ,
					 PURPLE);
			}

                        // translate to screen from world
			for (int line = 0; line < y_count; ++line) {
				DrawLine(screen_y_lines[line].start.x,
					 screen_y_lines[line].start.y,
					 screen_y_lines[line].end.x  ,
					 screen_y_lines[line].end.y  ,
					 RED);
			}

			for (int line = 0; line < x_count; ++line) {
				DrawLine(screen_x_lines[line].start.x,
					 screen_x_lines[line].start.y,
					 screen_x_lines[line].end.x  ,
					 screen_x_lines[line].end.y  ,
					 BLUE);
			}

                        DrawRectangleLinesRec(origin_rec, BROWN);
                        DrawRectangleLinesRec(translated_rec, YELLOW);
                        /* DrawRectangleLinesRec(rotated_rec, BLUE); */

                        DrawLineV(origin_line.start, origin_line.end, origin_line.color);                                     // Draw a line (using gl lines)
                        DrawLineV(rotated_line.start, rotated_line.end, rotated_line.color);                                     // Draw a line (using gl lines)
                        //
			DrawLine(vertical.start.x, vertical.start.y,
				 vertical.end.x, vertical.end.y,
				 vertical.color);

			DrawLine(horizontal.start.x, horizontal.start.y,
				 horizontal.end.x, horizontal.end.y,
				 horizontal.color);
		}

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
