#include <raylib.h>
#include "main.h"

void lab::draw(Segment s)
{
    const double h = GetRenderHeight();
    DrawLine(s.x1, h - s.y1, s.x2, h - s.y2, RAYWHITE);
    DrawCircle(s.x1, h - s.y1, 2, RAYWHITE);
    DrawCircle(s.x2, h - s.y2, 2, RAYWHITE);
}
void lab::draw(Rect r)
{
    const double h = GetRenderHeight();
    DrawRectangleLines(r.xmin, h - r.ymax, r.xmax - r.xmin, r.ymax - r.ymin, PINK);
}
void lab::draw(Point p)
{
    const static double S = 5;
    const double h = GetRenderHeight();
    DrawLineEx((Vector2){(float)(p.x - S), (float)(h - p.y - S)}, (Vector2){(float)(p.x + S), (float)(h - p.y + S)}, 2, RED);
    DrawLineEx((Vector2){(float)(p.x + S), (float)(h - p.y - S)}, (Vector2){(float)(p.x - S), (float)(h - p.y + S)}, 2, RED);
}
Point lab::mouse_point()
{
    const double h = GetRenderHeight();
    return (Point) {
        .x = (double)GetMouseX(),
        .y = h - (double)GetMouseY()
    };
}