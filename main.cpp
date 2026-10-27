#include "main.h"
#include <vector>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main()
{
    std::vector<Segment> segments;
    Rect cliprect = {0, 0, 800, 600};
    int number_of_segments_gen = 100;

    InitWindow(800, 600, "Raylib");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    std::optional<Point> buf;
    std::vector<Result> results;
    int results_i = -1;

    while (!WindowShouldClose()) {
        if (GuiButton((Rectangle){.x = 5, .y = 5, .width = 100, .height = 30}, "Uniform")) {
            cliprect = {0, 0, (double)GetRenderWidth(), (double)GetRenderHeight() - 70};
            segments = lab::gen_uniform_segments(cliprect, number_of_segments_gen, 42);
            results_i = -1;
        }
        if (GuiButton((Rectangle){.x = 110, .y = 5, .width = 100, .height = 30}, "Unequal")) {
            cliprect = {0, 0, (double)GetRenderWidth(), (double)GetRenderHeight() - 70};
            segments = lab::gen_unequal_segments(cliprect, number_of_segments_gen, 42);
            results_i = -1;
        }
        if (GuiButton((Rectangle){.x = 215, .y = 5, .width = 100, .height = 30}, "CALC")) {
            cliprect = {0, 0, (double)GetRenderWidth(), (double)GetRenderHeight() - 70};
            results = lab::calc_all_and_log_results(cliprect, segments);
            results_i = 0;
        }
        GuiValueBox((Rectangle){.x = 320, .y = 5, .width = 300, .height = 30}, "", &number_of_segments_gen, 10, 10000, true);

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            buf = lab::mouse_point();
        } else if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
            if (buf) {
                auto res = lab::segment_from_points(*buf, lab::mouse_point());
                if (res) {
                    auto clipped = lab::liang_barsky_clipper(*res, cliprect);
                    if (clipped) {
                        segments.push_back(*clipped);
                    }
                }
                buf = std::nullopt;
            }
        }
        if (results_i != -1) {
            if (IsKeyPressed(KEY_LEFT)) {
                results_i = (results_i + results.size() - 1) % results.size();
            } else if (IsKeyPressed(KEY_RIGHT)) {
                results_i = (results_i + 1) % results.size();
            }
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);
        // ====
        for (Segment s : segments) {
            lab::draw(s);
        }
        lab::draw(cliprect);
        if (buf) {
            auto mp = lab::mouse_point();
            auto mpline = lab::segment_from_points(mp, *buf);
            if (mpline) {
                lab::draw(*mpline);
                for (Segment s : segments) {
                    auto inter = lab::intersection(*mpline, s);
                    if (inter) {
                        lab::draw(*inter);
                    }
                }
            }
        }

        if (results_i != -1) {
            Result result = results[results_i];
            for (Rect rect : result.rects) {
                lab::draw(rect);
            }
            for (Point p : result.intersections) {
                lab::draw(p);
            }
            DrawText(TextFormat("%s took %.5fs. Result has %i points.\n", result.label.c_str(), (double)result.ns / 1000000000.0, (int)result.intersections.size()), 5, 35, 30, GREEN);
        }
        // ====
        EndDrawing();
    }

    CloseWindow();
}