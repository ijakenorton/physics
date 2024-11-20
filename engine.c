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
#define WIDTH (1920.0f)
#define HEIGHT (1080.0f)
#define lines_length 900
#define colors_length 10

#define DrawLineLine(line) DrawLineV(line->start, line->end, line->color)
Vector2 origin = { X_ORIGIN, Y_ORIGIN };
Vector2 centre = { WIDTH / 2.0f, HEIGHT / 2.0f };

static Arena default_arena = { 0 };
/* static Arena temporary_arena = { 0 }; */
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
	Vector2 v v.x = old.x + offset.x;
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

void spiral()
{
	Line_ptr *origin_along_x =
		l_init_def((Vector2){ 80.0f, 80.0f },
			   (Vector2){ 500.0f, Y_ORIGIN + 400.0f }, YELLOW);

	Line_ptr *rotating_lines[lines_length] = { 0 };
	float angle = 0.8f;
	float l_angle = 0.0f;

	Color colors[colors_length] = {
		LIGHTGRAY, GRAY, DARKGRAY, YELLOW, GOLD,
		ORANGE,	   PINK, RED,	   MAROON, GREEN,
	};
	for (size_t line = 0; line < lines_length; ++line) {
		rotating_lines[line] = l_world_to_screen(
			l_transform_deg(origin_along_x, l_angle));
		rotating_lines[line]->color = colors[line % 1];
		l_angle += angle;
	}

	InitWindow((int)WIDTH, (int)HEIGHT, "Physics Sandbox");
	SetTargetFPS(60);

	bool r_up = false;
	bool g_up = false;
	bool decreasing = true;

	float original_length = sqrtf(
		powf(rotating_lines[0]->end.x - rotating_lines[0]->start.x, 2) +
		powf(rotating_lines[0]->end.y - rotating_lines[0]->start.y, 2));

	const float max_length = 500.0f;
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
				if (original_length - angle <= 5.0f) {
					decreasing = false;
					original_length += angle;
				}
				original_length -= angle;

			} else {
				if (original_length + angle >= max_length) {
					decreasing = true;
					original_length -= angle;
				}
				original_length += angle;
			}

			l_screen_to_world_mut(rotating_lines[line]);
			l_transform_deg_mut(rotating_lines[line], angle,
					    original_length);
			l_translate_mut(rotating_lines[line],
					(Vector2){ 1.0f, 1.0f });
			l_world_to_screen_mut(rotating_lines[line]);

			if (r_up) {
				if (rotating_lines[line]->color.r +
					    (char)BLUE.r >
				    255) {
					r_up = false;
					rotating_lines[line]->color.r -=
						(char)(BLUE.r);
				}
				rotating_lines[line]->color.r += (char)1;

			} else {
				if (rotating_lines[line]->color.r -
						    (char)BLUE.r <
					    0 ||
				    rotating_lines[line]->color.r -
						    (char)BLUE.r >
					    rotating_lines[line]->color.r) {
					r_up = true;
					rotating_lines[line]->color.r +=
						(char)(BLUE.r);
				}
				rotating_lines[line]->color.r += (char)1;
			}
			if (g_up) {
				if (rotating_lines[line]->color.g +
					    (char)BLUE.g >
				    255) {
					g_up = false;
					rotating_lines[line]->color.g -=
						(char)(BLUE.g);
				}
				rotating_lines[line]->color.g += (char)1;

			} else {
				if (rotating_lines[line]->color.g -
						    (char)BLUE.g <
					    0 ||
				    rotating_lines[line]->color.g -
						    (char)BLUE.g >
					    rotating_lines[line]->color.g) {
					g_up = true;
					rotating_lines[line]->color.g +=
						(char)(BLUE.g);
				}
				rotating_lines[line]->color.g += (char)1;
			}
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

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED 1
void sprite_anim()
{
	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 800;
	const int screenHeight = 450;

	InitWindow(screenWidth, screenHeight,
		   "raylib [texture] example - sprite anim");

	// NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
	Texture2D scarfy =
		LoadTexture("resources/scarfy.png"); // Texture loading

	Vector2 position = { 350.0f, 280.0f };
	Rectangle frameRec = { 0.0f, 0.0f, (float)scarfy.width / 6,
			       (float)scarfy.height };

	int currentFrame = 0;

	int framesCounter = 0;
	int framesSpeed = 8; // Number of spritesheet frames shown by second

	SetTargetFPS(60); // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		framesCounter++;

		if (framesCounter >= (60 / framesSpeed)) {
			framesCounter = 0;
			currentFrame++;

			if (currentFrame > 5)
				currentFrame = 0;

			frameRec.x =
				(float)currentFrame * (float)scarfy.width / 6;
		}
		/* bool down = */

		if (IsKeyPressed(KEY_DOWN) || IsKeyDown(KEY_DOWN)) {
			print_float(position.y);
			position.y++;
		}
		if (IsKeyPressed(KEY_UP) || IsKeyDown(KEY_UP)) {
			print_float(position.y);
			position.y--;
		}
		// Control frames speed
		if (IsKeyPressed(KEY_RIGHT))
			framesSpeed++;
		else if (IsKeyPressed(KEY_LEFT))
			framesSpeed--;

		if (framesSpeed > MAX_FRAME_SPEED)
			framesSpeed = MAX_FRAME_SPEED;
		else if (framesSpeed < MIN_FRAME_SPEED)
			framesSpeed = MIN_FRAME_SPEED;
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		DrawTexture(scarfy, 15, 40, WHITE);
		DrawRectangleLines(15, 40, scarfy.width, scarfy.height, LIME);
		DrawRectangleLines(15 + (int)frameRec.x, 40 + (int)frameRec.y,
				   (int)frameRec.width, (int)frameRec.height,
				   RED);

		DrawText("FRAME SPEED: ", 165, 210, 10, DARKGRAY);
		DrawText(TextFormat("%02i FPS", framesSpeed), 575, 210, 10,
			 DARKGRAY);
		DrawText("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 290, 240, 10,
			 DARKGRAY);

		for (int i = 0; i < MAX_FRAME_SPEED; i++) {
			if (i < framesSpeed)
				DrawRectangle(250 + 21 * i, 205, 20, 20, RED);
			DrawRectangleLines(250 + 21 * i, 205, 20, 20, MAROON);
		}

		DrawTextureRec(scarfy, frameRec, position,
			       WHITE); // Draw part of the texture

		DrawText("(c) Scarfy sprite by Eiden Marsal", screenWidth - 200,
			 screenHeight - 20, 10, GRAY);

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	UnloadTexture(scarfy); // Texture unloading

	CloseWindow(); // Close window and OpenGL context
}

void simple_texture()
{
	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 320;
	const int screenHeight = 320;

	InitWindow(screenWidth, screenHeight, "Basic sprites");

	// NOTE: Be careful, background width must be equal or bigger than screen width
	// if not, texture should be draw more than two times for scrolling effect
	Texture2D all = LoadTexture("resources/space_assets.png");

	SetTargetFPS(60); // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(GetColor(0x052c46ff));

		// Draw background image twice
		// NOTE: Texture is scaled twice its size
		DrawTextureEx(all, (Vector2){ 20, 20 }, 0.0f, 0.5f, WHITE);
		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	UnloadTexture(all); // Unload background texture

	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
}
int main(void)
{
	/* spiral(); */
	/* sprite_explosion(); */
	/* simple_texture(); */
	sprite_anim();
}
