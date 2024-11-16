#include "raylib.h"
#include "types.h"
#include "collision.h"
#include <stdio.h>
#include <stdlib.h>
#define ARENA_IMPLEMENTATION
#include "arena.h"

/* float WIDTH =  1080.0f; */
/* float HEIGHT = 1080.0f; */

#define X_ORIGIN (0.0f)
#define Y_ORIGIN (0.0f)
#define WIDTH (1080.0f)
#define HEIGHT (1080.0f)
#define lines_length 100
#define colors_length 10

#define DrawLineLine(line) DrawLineV(line->start, line->end, line->color)
Vector2 origin = { X_ORIGIN, Y_ORIGIN };
Vector2 centre = { WIDTH / 2.0f, HEIGHT / 2.0f };

static Arena default_arena = { 0 };
static Arena temporary_arena = { 0 };
static Arena *context_arena = &default_arena;

void *context_alloc(size_t size)
{
	assert(context_arena);
	return arena_alloc(context_arena, size);
}

void *temp_alloc(Arena *temp, size_t size)
{
	context_arena = temp;
	void *new = context_alloc(size);
	context_arena = &default_arena;
	return new;
}

typedef struct {
	Vector2 start;
	Vector2 end;
	Color color;
} Line_ptr;

// Vector operations now return values instead of pointers
Vector2 v_translate(Vector2 old, Vector2 offset)
{
	Vector2 v;
	v.x = old.x + offset.x;
	v.y = old.y + offset.y;
	return v;
}

Vector2 v_mul(Vector2 old, float scalar)
{
	Vector2 v;
	v.x = old.x * scalar;
	v.y = old.y * scalar;
	return v;
}

Vector2 v_transform(Vector2 v, T_Matrix m)
{
	Vector2 new;
	new.x = (m.x1 * v.x) + (m.x2 * v.y);
	new.y = (m.y1 * v.x) + (m.y2 * v.y);
	return new;
}

Line_ptr *l_init(Vector2 start, Vector2 end, Color color, Arena *arena)
{
	Line_ptr *new = temp_alloc(arena, sizeof(Line_ptr));
	new->start = start;
	new->end = end;
	new->color = color;
	return new;
}

Line_ptr *l_init_def(Vector2 start, Vector2 end, Color color)
{
	return l_init(start, end, color, &default_arena);
}

Line_ptr *l_clone(Line_ptr *line, Arena *arena)
{
	Line_ptr *new = temp_alloc(arena, sizeof(Line_ptr));
	new->start = line->start;
	new->end = line->end;
	new->color = line->color;
	return new;
}

Line_ptr *l_clone_def(Line_ptr *line)
{
	return l_clone(line, &default_arena);
}

Line_ptr *l_translate(Line_ptr *line, Vector2 offset)
{
	Line_ptr *translated = l_clone_def(line);
	translated->start = v_translate(line->start, offset);
	translated->end = v_translate(line->end, offset);
	return translated;
}

Line_ptr *l_transform(Line_ptr *line, T_Matrix m)
{
	Line_ptr *translated = l_clone_def(line);
	translated->start = v_transform(line->start, m);
	translated->end = v_transform(line->end, m);
	return translated;
}

Line_ptr *l_transform_deg(Line_ptr *line, float deg)
{
	T_Matrix m;
	m.x1 = cosf(DEG2RAD * deg);
	m.y1 = -sinf(DEG2RAD * deg);
	m.x2 = sinf(DEG2RAD * deg);
	m.y2 = cosf(DEG2RAD * deg);

	Line_ptr *translated = l_clone_def(line);
	translated->start = v_transform(line->start, m);
	translated->end = v_transform(line->end, m);
	return translated;
}

Line_ptr *l_world_to_screen(Line_ptr *line)
{
	return l_translate(line, centre);
}

Line_ptr *l_screen_to_world(Line_ptr *line)
{
	return l_translate(line, v_mul(centre, -1.0f));
}

void v_translate_mut(Vector2 *v, Vector2 offset)
{
	v->x += offset.x;
	v->y += offset.y;
}

void v_transform_mut(Vector2 *v, T_Matrix m)
{
	v->x = (m.x1 * v->x) + (m.x2 * v->y);
	v->y = (m.y1 * v->x) + (m.y2 * v->y);
}

void l_translate_mut(Line_ptr *line, Vector2 offset)
{
	v_translate_mut(&line->start, offset);
	v_translate_mut(&line->end, offset);
}

void l_world_to_screen_mut(Line_ptr *line)
{
	l_translate_mut(line, centre);
}

void l_screen_to_world_mut(Line_ptr *line)
{
	l_translate_mut(line, v_mul(centre, -1.0f));
}

float normalize_line_length(Line_ptr *line, float target_length)
{
	Vector2 diff = { line->end.x - line->start.x,
			 line->end.y - line->start.y };
	float current_length = sqrtf(diff.x * diff.x + diff.y * diff.y);
	float scale = target_length / current_length;

	// Keep start point fixed, scale end point
	line->end.x = line->start.x + (diff.x * scale);
	line->end.y = line->start.y + (diff.y * scale);
	return current_length;
}

void l_transform_deg_mut(Line_ptr *line, float deg, float original_length)
{
	T_Matrix m;
	m.x1 = cosf(DEG2RAD * deg);
	m.y1 = -sinf(DEG2RAD * deg);
	m.x2 = sinf(DEG2RAD * deg);
	m.y2 = cosf(DEG2RAD * deg);

	v_transform_mut(&line->start, m);
	v_transform_mut(&line->end, m);
	normalize_line_length(line, original_length);
}

bool color_equals(Color a, Color b)
{
	return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

void assert_with_message(bool condition, char *message)
{
	print_int(condition);
	if (!condition) {
		fprintf(stderr, "%s", message);
		exit(1);
	}
}

void GameLoop()
{
	Line_ptr *horizontal = l_init_def((Vector2){ 0.0f, HEIGHT / 2.0f },
					  (Vector2){ WIDTH, HEIGHT / 2.0f },
					  WHITE);

	Line_ptr *vertical = l_init_def((Vector2){ WIDTH / 2.0f, HEIGHT },
					(Vector2){ WIDTH / 2.0f, 0.0f }, WHITE);

	Line_ptr *origin_along_x =
		l_init_def((Vector2){ 50.0f, 50.0f },
			   (Vector2){ 800.0f, Y_ORIGIN + 400.0f }, YELLOW);

	Line_ptr *rotating_lines[lines_length] = { 0 };
	float angle = 3.6f;
	float l_angle = 0.0f;

	Color colors[colors_length] = {
		LIGHTGRAY, GRAY, DARKGRAY, YELLOW, GOLD,
		ORANGE,	   PINK, RED,	   MAROON, GREEN,
	};
	for (size_t line = 0; line < lines_length; ++line) {
		rotating_lines[line] = l_world_to_screen(
			l_transform_deg(origin_along_x, l_angle));
		/* rotating_lines[line]->color = colors[line % 2]; */
		rotating_lines[line]->color = line % 2 == 0 ? BLUE : GREEN;
		l_angle += angle;
	}

	InitWindow((int)WIDTH, (int)HEIGHT, "Physics Sandbox");
	SetTargetFPS(60);

	bool r_up = false;
	bool g_up = false;
	bool b_up = false;
	bool decreasing = true;

	float original_length = sqrtf(
		powf(rotating_lines[0]->end.x - rotating_lines[0]->start.x, 2) +
		powf(rotating_lines[0]->end.y - rotating_lines[0]->start.y, 2));

	const float max_length = 800.0f;
	print_float(max_length);

	while (!WindowShouldClose()) {
		for (int line = 0; line < lines_length; ++line) {
			original_length = sqrtf(
				powf(rotating_lines[line]->end.x -
					     rotating_lines[line]->start.x,
				     2) +
				powf(rotating_lines[line]->end.y -
					     rotating_lines[line]->start.y,
				     2));
			if (decreasing) {
				rotating_lines[line]->color =
					color_equals(
						rotating_lines[line]->color,
						GREEN) ?
						BLUE :
						GREEN;
				if (original_length - angle <= 5.0f) {
					decreasing = false;
					original_length += angle;
				}
				original_length -= angle;

			} else {
				rotating_lines[line]->color =
					color_equals(
						rotating_lines[line]->color,
						BLUE) ?
						YELLOW :
						RED;

				if (original_length + angle >= max_length) {
					decreasing = true;
					original_length -= angle;
				}
				original_length += angle;
			}

			l_screen_to_world_mut(rotating_lines[line]);
			l_transform_deg_mut(rotating_lines[line], angle,
					    original_length);
			l_world_to_screen_mut(rotating_lines[line]);

			/* if (r_up) { */
			/* 	if (rotating_lines[line]->color.r + */
			/* 		    (char)BLUE.r > */
			/* 	    255) { */
			/* 		r_up = false; */
			/* 		rotating_lines[line]->color.r -= */
			/* 			(char)(BLUE.r); */
			/* 	} */
			/* 	rotating_lines[line]->color.r += (char)(BLUE.r); */

			/* } else { */
			/* 	if (rotating_lines[line]->color.r - */
			/* 			    (char)BLUE.r < */
			/* 		    0 || */
			/* 	    rotating_lines[line]->color.r - */
			/* 			    (char)BLUE.r > */
			/* 		    rotating_lines[line]->color.r) { */
			/* 		r_up = true; */
			/* 		rotating_lines[line]->color.r += */
			/* 			(char)(BLUE.r); */
			/* 	} */
			/* 	rotating_lines[line]->color.r += (char)(BLUE.r); */
			/* } */
			/* if (g_up) { */
			/* 	if (rotating_lines[line]->color.g + */
			/* 		    (char)BLUE.g > */
			/* 	    255) { */
			/* 		g_up = false; */
			/* 		rotating_lines[line]->color.g -= */
			/* 			(char)(BLUE.g); */
			/* 	} */
			/* 	rotating_lines[line]->color.g += (char)(BLUE.g); */

			/* } else { */
			/* 	if (rotating_lines[line]->color.g - */
			/* 			    (char)BLUE.g < */
			/* 		    0 || */
			/* 	    rotating_lines[line]->color.g - */
			/* 			    (char)BLUE.g > */
			/* 		    rotating_lines[line]->color.g) { */
			/* 		g_up = true; */
			/* 		rotating_lines[line]->color.g += */
			/* 			(char)(BLUE.g); */
			/* 	} */
			/* 	rotating_lines[line]->color.g += (char)(BLUE.g); */
			/* } */

			/* if (b_up) { */
			/* 	if (rotating_lines[line]->color.b + */
			/* 		    (char)BLUE.b > */
			/* 	    255) { */
			/* 		b_up = false; */
			/* 		rotating_lines[line]->color.b -= */
			/* 			(char)(BLUE.b); */
			/* 	} */
			/* 	rotating_lines[line]->color.b += (char)(BLUE.b); */

			/* } else { */
			/* 	if (rotating_lines[line]->color.b - */
			/* 			    (char)BLUE.b < */
			/* 		    0 || */
			/* 	    rotating_lines[line]->color.b - */
			/* 			    (char)BLUE.b > */
			/* 		    rotating_lines[line]->color.b) { */
			/* 		b_up = true; */
			/* 		rotating_lines[line]->color.b += */
			/* 			(char)(BLUE.b); */
			/* 	} */
			/* 	rotating_lines[line]->color.b += (char)(BLUE.b); */
			/* } */
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (int line = 0; line < lines_length; ++line) {
			DrawLineLine(rotating_lines[line]);
		}

		EndDrawing();
	}

	CloseWindow();
}

int main(void)
{
	GameLoop();
}
