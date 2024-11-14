#include "raylib.h"
#include "types.h"
#include "collision.h"
#include <stdio.h>
#include <stdlib.h>
#define ARENA_IMPLEMENTATION
#include "arena.h"

float screen_width = 1080.0f ;
float screen_height = 1080.0f;

/* #define DrawRectangleLinesRec(rec, color) \ */
/* 	DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, color) */
                        
#define DrawLineLine(line) DrawLineV(line.start, line.end, line.color)
#define DrawLineLine_t(line) DrawLineV(*line->start, *line->end, line->color)
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

Vector2 *world_to_screen_v( Vector2 *old_v)
{

        Vector2 *p = malloc(sizeof(Vector2));
        p->x = old_v->x - screen_width / 2.0f; 
        p->y = old_v->y - screen_height/ 2.0f;
        return p;
}

Vector2 *screen_to_world_v(Vector2 *new_v, Vector2 *old_v)
{
        Vector2 *p = context_alloc(sizeof(Vector2));
        p->x = old_v->x + screen_width / 2.0f; 
        p->y = old_v->y + screen_height/ 2.0f;
        return p;
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


Line_t line_screen_to_world_v(Line_t old_l, Translation_Matrix m)
{
        Line_t l = context_alloc(sizeof(Line_t));
        l->start = vec2_linear_translate(*old_l->start, m);
        l->end = vec2_linear_translate(*old_l->end, m);
        return l;

}


void vec2_linear_translate_old(Vector2 *new_v, Vector2 *old_v, Translation_Matrix m)
{
        new_v->x = (m.x1 * old_v->x) + (m.x2 * old_v->x);
        new_v->y = (m.y1 * old_v->y) + (m.y2 * old_v->y);
}


Vector2 *vec2_linear_translate(Vector2 v, Translation_Matrix m)
{
        Vector2 *new = context_alloc(sizeof(Vector2));
        new->x = (m.x1 * v.x) + (m.x2 * v.x);
        new->y = (m.y1 * v.y) + (m.y2 * v.y);
        return new;
}

Line_t vec2_linear_translate_line(Line_t old_l, Translation_Matrix m)
{
        Line_t l = context_alloc(sizeof(Line_t));
        l->start = vec2_linear_translate(*old_l->start, m);
        l->end = vec2_linear_translate(*old_l->end, m);
        return l;

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

        Line_t temp_l = context_alloc(sizeof(Line_t));
        temp_l->start = context_alloc(sizeof(Vector2 *));
        temp_l->end = context_alloc(sizeof(Vector2 *));

	Line_t rotated_y_lines[RULER_INC] = { 0 };
	for (int i = 0; i < y_count; ++i ) {
                temp_l->start = &screen_y_lines[i].start;
                temp_l->end = &screen_y_lines[i].end;
                temp_l->color = BLUE;

		rotated_y_lines[i] = temp_l;
		rotated_y_lines[i] = vec2_linear_translate_line(temp_l, (Translation_Matrix){.x1 = 0.0f, .y1 = 1.0f, .x2 = -1.0f, .y2 = 0.0f});
                print_line_t(rotated_y_lines[i]);
	}

	/* Line rotated_x_lines[RULER_INC] = { 0 }; */
	/* for (int i = 0; i < x_count; ++i ) { */
	/* 	rotated_x_lines[i] = vec2_linear_translate_line(&screen_x_lines[i]); */
	/* } */



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

        Vector2 origin = { 0.0, 0.0 };
        Vector2 line_offset = { 50.0f, 50.f };


	InitWindow((int)screen_width, (int)screen_height, "Physics stuff");

	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(BLACK);
		// update
		// ------------------------------------------------
		// Bit of a hack to fix space being triggered multiple times
		// Handles pause

		if (debug) {

			/* for (int line = 0; line < y_count; ++line) { */
                                /* DrawLineLine(y_lines[line]); */
			/* } */

			/* for (int line = 0; line < x_count; ++line) { */
			/* 	DrawLineLine(x_lines[line]); */
			/* } */

                        /* /1* // translate to world from screen *1/ */
			/* for (int line = 0; line < y_count; ++line) { */
			/* 	DrawLineLine(world_y_lines[line]); */
			/* } */

			for (int line = 0; line < y_count; ++line) {
				DrawLineLine_t(rotated_y_lines[line]);
			}

			/* for (int line = 0; line < x_count; ++line) { */
			/* 	DrawLineLine(world_x_lines[line]); */
			/* } */

                        /* // translate to screen from world */
			/* for (int line = 0; line < y_count; ++line) { */
			/* 	DrawLineLine(screen_y_lines[line]); */
			/* } */

			/* for (int line = 0; line < x_count; ++line) { */
			/* 	DrawLineLine(screen_x_lines[line]); */
			/* } */

                        /* DrawRectangleLinesRec(origin_rec, BROWN); */
                        /* DrawRectangleLinesRec(translated_rec, YELLOW); */

                        /* DrawRectangleLinesRec(rotated_rec, BLUE); */

                        DrawLineV(origin, line_offset, PURPLE);

                        //
			DrawLineLine(vertical);

			DrawLineLine(horizontal);
		}

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
