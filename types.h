#ifndef TYPES_H
#define TYPES_H
#include <stdbool.h>
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>

#define NUM_ROW 20
#define RULER_INC 96
#define MAX_LEVEL 2
#define expand_vec2(vec) vec.x, vec.y
#define expand_vec2_p(vec) vec->x, vec->y
#define expand_rec(rec) rec.x, rec.y, rec.width, rec.height
#define print_int(val) printf(#val " = %d\n", val)
#define print_zu(val) printf(#val " = %zu\n", val)
#define print_float(val) printf(#val " = %f\n", val)
#define print_float_v2(val) \
	printf(#val "\n.x = %f\n.y = %f\n", expand_vec2(val))
#define print_v2_p(val) printf(#val "\n.x = %f\n.y = %f\n", expand_vec2_p(val))
#define print_line(line)                                                       \
	printf(#line "\nstart.x = %f\nstart.y = %f\nend.x = %f\nend.y = %f\n", \
	       expand_vec2(line.start), expand_vec2(line.end))
#define print_line_t(line)                                                   \
	printf(#line                                                         \
	       "\nstart->x = %f\nstart->y = %f\nend->x = %f\nend->y = %f\n", \
	       line->start->x, line->start->y, line->end->x, line->end->y)
#define print_rec(rec) \
	printf("x = %f\ny = %f\nwidth = %f\nheight = %f\n", expand_rec(rec))
#define print_ball(ball)                                                                                                            \
	printf("centre.x = %f\ncentre.y = %f\nvelocity.x = %f\nvelocity.y = %f\ndirection.x = %f\ndirection.y = %f\nradius = %f\n", \
	       expand_vec2(ball->centre), expand_vec2(ball->velocity),

#define print_block(block)                                           \
	printf("x = %f y = %f\nwidth = %f height = %f alive = %d\n", \
	       expand_rec(block.rec), block.alive)

#define DrawRectangleLinesRec(rec, color) \
	DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, color)

#define expand_color(color) color.r, color.g, color.b, color.a
#define expand_color_p(color) color->r, color->g, color->b, color->a
#define print_color(color) \
	printf(#color "= { %u, %u, %u, %u }\n", expand_color(color))

typedef struct {
	float x1;
	float y1;
	float x2;
	float y2;
} T_Matrix;

typedef struct {
	Vector2 start;
	Vector2 end;
	Color color;
} Line;

typedef struct {
	Vector2 *start;
	Vector2 *end;
	Color color;
} *Line_t;

typedef struct {
	Rectangle rec;
	bool alive;
	Color main_color;
	Color outline_color;
} Block;

typedef struct {
	Block *blocks;
	int num_alive;
	int num_blocks;
} BlocksState;

typedef struct {
	Vector2 centre;
	Vector2 velocity;
	float radius;
	Color color;
} Ball;

typedef struct {
	Rectangle *rec;
	Vector2 *origin;
	float rotation;
} Paddle;

typedef struct {
	BlocksState *blocks_state;
	Ball *ball;
	Paddle *paddle;
} Level;

typedef struct {
	bool loss;
	bool win;
	bool paused;
	Level *cl;
	int level_no;
} State;

typedef void (*LevelFunc)(Level *level, State *state);

typedef enum {
	LEFT = 0,
	RIGHT = 1,
	TOP = 2,
	BOTTOM = 3,
} DIRECTION;

typedef struct {
	DIRECTION d;
	float testX;
	float testY;
} Line_id;

#endif // !TYPES_H
