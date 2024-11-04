#include <stdio.h>
#include <math.h>
#include <stdbool.h>

typedef struct Rectangle {
	float x; // Rectangle top-left corner position x
	float y; // Rectangle top-left corner position y
	float width; // Rectangle width
	float height; // Rectangle height
} Rectangle;

typedef struct Vector2 {
	float x; // Vector x component
	float y; // Vector y component
} Vector2;

typedef struct {
	Vector2 centre;
	Vector2 velocity;
	Vector2 direction;

	float radius;
	/* Color color; */
} Ball;

bool pointPoint(Vector2 point1, Vector2 point2)
{
	if (point1.x == point2.x && point1.y == point2.y) {
		return true;
	}
	return false;
}

float dist(Vector2 point1, Vector2 point2)
{
	float distX = point1.x - point2.x;
	float distY = point1.y - point2.y;
	return sqrtf((distX * distX) + (distY * distY));
}

bool point_circle(Vector2 point, Ball c)
{
	float distance = dist(point, c.centre);
	if (distance <= c.radius) {
		return true;
	}
	return false;
}

bool circle_circle(Ball circle1, Ball circle2)
{
	float distance = dist(circle1.centre, circle2.centre);
	if (distance <= circle1.radius + circle2.radius) {
		return true;
	}
	return false;
}

bool point_rect(Vector2 point, Rectangle rect)
{
	if (point.x >= rect.x && point.x <= rect.x + rect.width &&
	    point.y >= rect.y && point.y <= rect.y + rect.height) {
		return true;
	}
	return false;
}

bool rect_rect(Rectangle r1, Rectangle r2)
{
	if (r1.x + r1.width >= r2.x && // r1 right edge past r2 left
	    r1.x <= r2.x + r2.width && // r1 left edge past r2 right
	    r1.y + r1.height >= r2.y && // r1 top edge past r2 bottom
	    r1.y <= r2.y + r2.height) { // r1 bottom edge past r2 top
		return true;
	}
	return false;
}

bool circle_rect(Ball c, Rectangle r)
{
	// temporary variables to set edges for testing
	float testX = c.centre.x;
	float testY = c.centre.y;

	// which edge is closest?
	if (c.centre.x < r.x)
		testX = r.x; // test left edge
	else if (c.centre.x > r.x + r.width)
		testX = r.x + r.width; // right edge
	if (c.centre.y < r.y)
		testY = r.y; // top edge
	else if (c.centre.y > r.y + r.height)
		testY = r.y + r.height; // bottom edge

	// get distance from closest edges
	float distX = c.centre.x - testX;
	float distY = c.centre.y - testY;
	float distance = sqrt((distX * distX) + (distY * distY));

	// if the distance is less than the radius, collision!
	if (distance <= c.radius) {
		return true;
	}
	return false;
}

bool line_point(Vector2 l_start, Vector2 l_end, Vector2 p)
{
	// get distance from the point to the two ends of the line
	float d1 = dist(p, l_start);
	float d2 = dist(p, l_end);

	// get the length of the line
	float lineLen = dist(l_start, l_end);

	// since floats are so minutely accurate, add
	// a little buffer zone that will give collision
	float buffer = 0.1; // higher # = less accurate

	// if the two distances are equal to the line's
	// length, the point is on the line!
	// note we use the buffer here to give a range,
	// rather than one #
	if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer) {
		return true;
	}
	return false;
}

// LINE/CIRCLE
bool line_circle(Vector2 l_start, Vector2 l_end, Ball c)
{
	// is either end INSIDE the circle?
	// if so, return true immediately
	bool inside1 = point_circle(l_start, c);
	bool inside2 = point_circle(l_end, c);
	if (inside1 || inside2)
		return true;

	// get length of the line
	float distX = l_start.x - l_end.x;
	float distY = l_start.y - l_end.y;
	float len = sqrt((distX * distX) + (distY * distY));

	// get dot product of the line and circle
	float dot = (((c.centre.x - l_start.x) * (l_end.x - l_start.x)) +
		     ((c.centre.y - l_start.y) * (l_end.y - l_start.y))) /
		    pow(len, 2);

	// find the closest point on the line
	float closestX = l_start.x + (dot * (l_end.x - l_start.x));
	float closestY = l_start.y + (dot * (l_end.y - l_start.y));

	// is this point actually on the line segment?
	// if so keep going, but if not, return false
	bool onSegment = line_point(l_start, l_end,
				    (Vector2){ .x = closestX, .y = closestY });
	if (!onSegment)
		return false;

	// get distance to closest point
	distX = closestX - c.centre.x;
	distY = closestY - c.centre.y;
	float distance = sqrt((distX * distX) + (distY * distY));

	if (distance <= c.radius) {
		return true;
	}
	return false;
}

int main(void)
{
	Vector2 point1 = { 10.0f, 10.0f };
	Vector2 point2 = { 40.0f, 50.0f };

	float distance = dist(point1, point2);
	printf("dist: %f", distance);
}
