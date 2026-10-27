#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <optional>
#include <vector>
#include <cstdint>

struct Segment
{
    double x1, y1, x2, y2;
};

struct Rect
{
    double xmin, ymin, xmax, ymax;
};

struct Point
{
    double x, y;
};

// struct SmartDelimitResult
// {
//     std::vector<Rect> rects;
//     std::vector<int> remaining_indexes; // only need to calc between these afterwards. That's because ALL collisions inside rects will be accounted for.
// };

struct Result
{
    std::string label;
    std::vector<Point> intersections;
    std::vector<Rect> rects;
    std::uint64_t ns;
};

namespace lab {
    std::optional<Segment> liang_barsky_clipper(Segment s, Rect r);
    std::optional<Point> intersection(Segment s1, Segment s2);
    std::optional<Segment> segment_from_points(Point p1, Point p2);
    Rect expand(Rect r1, Rect r2);
    
    std::vector<Segment> gen_uniform_segments(Rect bounds, int count, int seed);
    std::vector<Segment> gen_unequal_segments(Rect bounds, int count, int seed);

    std::vector<Rect> dumb_delimit(Rect r, int cnt);
    //SmartDelimitResult smart_delimit(const std::vector<Segment>& segments);
    std::vector<Rect> smarter_delimit(const std::vector<Segment> &segments, Rect rect, int cnt);
    std::vector<Point> calc_dumbest_intersections(const std::vector<Segment> &segments);
    std::vector<Point> calc_dumb_intersections(const std::vector<Segment>& segments, std::vector<Rect> rects, int start_index, int end_index);
    std::vector<Point> calc_smarter_intersections(const std::vector<Segment>& segments, std::vector<Rect> rects, int start_index, int end_index);

    std::vector<Result> calc_all_and_log_results(Rect r, const std::vector<Segment> &segments);

    void draw(Segment s);
    void draw(Rect r);
    void draw(Point p);
    Point mouse_point();
}