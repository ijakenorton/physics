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

#define DrawLineLine(line) DrawLineV(*line->start, *line->end, line->color)
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

Vector2 *v_clone(Vector2 vec, Arena *arena)
{
	Vector2 *new = temp_alloc(arena, sizeof(Vector2));
	new->x = vec.x;
	new->y = vec.y;
	return new;
}

Vector2 *v_clone_def(Vector2 vec)
{
	return v_clone(vec, &default_arena);
}

typedef struct {
	Vector2 *start;
	Vector2 *end;
	Color color;
} Line_ptr;

Line_ptr *l_init(Vector2 start, Vector2 end, Color color, Arena *arena)
{
	Line_ptr *new = temp_alloc(arena, sizeof(Line_ptr));
	new->start = v_clone(start, arena);
	new->end = v_clone(end, arena);
	new->color = color;
	return new;
}

Line_ptr *l_init_def(Vector2 start, Vector2 end, Color color)
{
	Line_ptr *new = temp_alloc(&default_arena, sizeof(Line_ptr));
	new->start = v_clone_def(start);
	new->end = v_clone_def(end);
	new->color = color;
	return new;
}

Line_ptr *l_clone(Line_ptr *line, Arena *arena)
{
	Line_ptr *new = temp_alloc(arena, sizeof(Line_ptr));
	new->start = v_clone(*line->start, arena);
	new->end = v_clone(*line->end, arena);
	return new;
}

Line_ptr *l_clone_def(Line_ptr *line)
{
	Line_ptr *new = context_alloc(sizeof(Line_ptr));
	new->start = v_clone_def(*line->start);
	new->end = v_clone_def(*line->end);
	return new;
}

Vector2 *v_translate(Vector2 old, Vector2 offset)
{
	Vector2 *v = v_clone_def(old);
	v->x += offset.x;
	v->y += offset.y;
	return v;
}

Line_ptr *l_translate(Line_ptr *line, Vector2 offset)
{
	Line_ptr *translated = l_clone_def(line);
	translated->start = v_translate(*line->start, offset);
	translated->end = v_translate(*line->end, offset);
	return translated;
}

Vector2 *v_transform(Vector2 v, T_Matrix m)
{
	Vector2 *new = temp_alloc(&default_arena, sizeof(Vector2));
	new->x = (m.x1 * v.x) + (m.x2 * v.x);
	new->y = (m.y1 * v.y) + (m.y2 * v.y);
	return new;
}

Line_ptr *l_transform(Line_ptr *line, T_Matrix m)
{
	Line_ptr *translated = l_clone_def(line);
	translated->start = v_transform(*line->start, m);
	translated->end = v_transform(*line->end, m);
	return translated;
}

Line_ptr *l_world_to_screen(Line_ptr *line)
{
	return l_translate(line, centre);
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
			   (Vector2){ 100.0f, Y_ORIGIN + 50.0f }, YELLOW);

	Line_ptr *t_origin = l_translate(origin_along_x, centre);
	T_Matrix rotation_m =
		(T_Matrix){ .x1 = 0.0f, .y1 = 1.0f, .x2 = -1.0f, .y2 = 0.0f };
	Line_ptr *transform_origin =
		l_world_to_screen(l_transform(origin_along_x, rotation_m));
	transform_origin->color = YELLOW;

	t_origin->color = WHITE;

	print_line_t(origin_along_x);
	print_line_t(t_origin);
	print_line_t(transform_origin);

	InitWindow((int)WIDTH, (int)HEIGHT, "Physics Sandbox");

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		DrawLineLine(horizontal);
		DrawLineLine(vertical);
		DrawLineLine(origin_along_x);
		DrawLineLine(t_origin);
		DrawLineLine(transform_origin);

		EndDrawing();
	}

	CloseWindow();
}

int main(void)
{
	GameLoop();

	/* Vector2 *v = temp_alloc(&temporary_arena, sizeof(Vector2 *)); */
	/* *v = origin; */
	/* Vector2 *cl = l_clone_def(v); */
	/* cl->x = 10.0f; */
	/* cl->y = 10.0f; */

	/* print_float_v2(origin); */
	/* print_v2_p(v); */
	/* print_v2_p(cl); */
	/* print_v2_p(v); */

	/* printf("hello world\n"); */
}
