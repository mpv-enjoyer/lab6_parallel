#include "main.h"
#include <functional>
#include <algorithm>
#include <numeric>
#include <future>
#include "debug_output.hpp"
// using namespace std;

// this function gives the maximum
static double maxi(double arr[], int n) {
    double m = 0;
    for (int i = 0; i < n; ++i)
    if (m < arr[i])
    m = arr[i];
    return m;
}

// this function gives the minimum
static double mini(double arr[], int n) {
    double m = 1;
    for (int i = 0; i < n; ++i)
    if (m > arr[i])
    m = arr[i];
    return m;
}

// Liang–Barsky line-clipping algorithm
std::optional<Segment> lab::liang_barsky_clipper(Segment s, Rect r) {
  // defining variables
  double p1 = -(s.x2 - s.x1);
  double p2 = -p1;
  double p3 = -(s.y2 - s.y1);
  double p4 = -p3;

  double q1 = s.x1 - r.xmin;
  double q2 = r.xmax - s.x1;
  double q3 = s.y1 - r.ymin;
  double q4 = r.ymax - s.y1;

  double exitParams[5], entryParams[5];
  int exitIndex = 1, entryIndex = 1;
  exitParams[0] = 1;
  entryParams[0] = 0;

//   rectangle(xmin, ymin, xmax, ymax); // drawing the clipping window

  if ((p1 == 0 && q1 < 0) || (p2 == 0 && q2 < 0) || (p3 == 0 && q3 < 0) || (p4 == 0 && q4 < 0)) {
    //   outtextxy(80, 80, "Line is parallel to clipping window!");
      return std::nullopt;
  }
  if (p1 != 0) {
    double r1 = q1 / p1;
    double r2 = q2 / p2;
    if (p1 < 0) {
      entryParams[entryIndex++] = r1;
      exitParams[exitIndex++] = r2;
    } else {
      entryParams[entryIndex++] = r2;
      exitParams[exitIndex++] = r1;
    }
  }
  if (p3 != 0) {
    double r3 = q3 / p3;
    double r4 = q4 / p4;
    if (p3 < 0) {
      entryParams[entryIndex++] = r3;
      exitParams[exitIndex++] = r4;
    } else {
      entryParams[entryIndex++] = r4;
      exitParams[exitIndex++] = r3;
    }
  }

  double clippedX1, clippedY1, clippedX2, clippedY2;
  double u1, u2;
  u1 = maxi(entryParams, entryIndex); // maximum of entry points
  u2 = mini(exitParams, exitIndex);   // minimum of exit points

  if (u1 > u2) {
    // outtextxy(80, 80, "Line is outside the clipping window!");
    return std::nullopt;
  }

  clippedX1 = s.x1 + (s.x2 - s.x1) * u1;
  clippedY1 = s.y1 + (s.y2 - s.y1) * u1;
  clippedX2 = s.x1 + (s.x2 - s.x1) * u2;
  clippedY2 = s.y1 + (s.y2 - s.y1) * u2;

  return (Segment) {
    .x1 = clippedX1,
    .y1 = clippedY1,
    .x2 = clippedX2,
    .y2 = clippedY2
  };
}

struct CramerResult {
    double s, t;
    bool valid;
};
// a1s + b1t = c2
// a2s + b2t = c2
static CramerResult cramer(double a1, double b1, double c1, double a2, double b2, double c2) {
    double det = (a1 * b2) - (b1 * a2);
    if (det == 0) {
        return (CramerResult){
            .valid = false
        };
    }
    return (CramerResult) {
        .s = ((c1 * b2) - (b1 * c2)) / det,
        .t = ((a1 * c2) - (c1 * a2)) / det,
        .valid = true
    };
}

static bool point_in_segment(Point p, Segment s) {
    double sminx = std::min(s.x1, s.x2);
    double sminy = std::min(s.y1, s.y2);
    double smaxx = std::max(s.x1, s.x2);
    double smaxy = std::max(s.y1, s.y2);
    if (p.x > smaxx) return false;
    if (p.x < sminx) return false;
    if (p.y > smaxy) return false;
    if (p.y < sminy) return false;
    return true;
};

// TODO: multiple points of intersection?? Do I need to handle that?

std::optional<Point> lab::intersection(Segment s1, Segment s2)
{
    // https://en.wikipedia.org/wiki/Intersection_(geometry)#Two_line_segments
    double x1 = s1.x1;
    double x2 = s1.x2;
    double x3 = s2.x1;
    double x4 = s2.x2;
    double y1 = s1.y1;
    double y2 = s1.y2;
    double y3 = s2.y1;
    double y4 = s2.y2;
    CramerResult c = cramer(x2 - x1, x4 - x3, x3 - x1, y2 - y1, y4 - y3, y3 - y1);
    if (c.valid) {
        // printf("cvalid\n");
        Point r = (Point) {
            .x = x1 + c.s * (x2 - x1),
            .y = y1 + c.s * (y2 - y1)
        };

        if (point_in_segment(r, s1) && point_in_segment(r, s2)) {
            // printf("intersect!\n");
            return r;
        } else {
            return std::nullopt;
        }
    } else {
        // printf("cinvalid\n");
        return std::nullopt;
    }
}

std::optional<Segment> lab::segment_from_points(Point p1, Point p2)
{
    if (p1.x == p2.x && p1.y == p2.y) {
        return std::nullopt;
    } else {
        return (Segment) {
            .x1 = p1.x,
            .y1 = p1.y,
            .x2 = p2.x,
            .y2 = p2.y
        };
    }
}

Rect rect_from_segment(Segment s)
{
    return (Rect){
        .xmin = std::min(s.x1, s.x2),
        .ymin = std::min(s.y1, s.y2),
        .xmax = std::max(s.x1, s.x2),
        .ymax = std::max(s.y1, s.y2),
    };
}

bool are_rects_intersecting(Rect rec1, Rect rec2)
{
    bool collision = false;

    if ((rec1.xmin <= rec2.xmax && rec1.xmax >= rec2.xmin) &&
        (rec1.ymin <= rec2.ymax && rec1.ymax >= rec2.ymin)) collision = true;

    return collision;
}

// Must collide:
Rect lab::expand(Rect r1, Rect r2)
{
    return (Rect) {
        .xmin = std::min(r1.xmin, r2.xmin),
        .ymin = std::min(r1.ymin, r2.ymin),
        .xmax = std::max(r1.xmax, r2.xmax),
        .ymax = std::max(r1.ymax, r2.ymax),
    };
}

static bool are_point_and_rect_intersecting(Point point, Rect rec)
{
    bool collision = false;

    if ((point.x >= rec.xmin) && (point.x <= rec.xmax) && (point.y >= rec.ymin) && (point.y <= rec.ymax)) collision = true;

    return collision;
}

// [min, max]
static double gen_random_double(double min, double max) {
    static_assert(RAND_MAX == __INT32_MAX__);
    return ((double)rand() / RAND_MAX) * (max - min) + min;
}

// [min, max)
static int gen_random_int(int min, int max) {
    static_assert(RAND_MAX == __INT32_MAX__);
    return (rand() % (max - min)) + min;
}

static Segment gen_random_segment(Rect bounds) {
    const double MIN_LEN = std::min(bounds.ymax - bounds.ymin, bounds.xmax - bounds.xmin) / 150.0;
    const double MAX_LEN = std::min(bounds.ymax - bounds.ymin, bounds.xmax - bounds.xmin) / 3.0;

    const double len = gen_random_double(MIN_LEN, MAX_LEN);
    const Point start = (Point) {
        .x = gen_random_double(bounds.xmin, bounds.xmax),
        .y = gen_random_double(bounds.ymin, bounds.ymax),
    };
    while (true) {
        double angle = gen_random_double(0, 2 * M_PI);
        const Point end = (Point) {
            .x = start.x + len * std::cos(angle),
            .y = start.y + len * std::sin(angle)
        };
        if (are_point_and_rect_intersecting(end, bounds)) {
            return *lab::segment_from_points(start, end);
        }
    }
}

std::vector<Segment> lab::gen_uniform_segments(Rect bounds, int count, int seed)
{
    srand(seed);
    std::vector<Segment> output(count);

    for (int i = 0; i < count; i++) {
        output[i] = gen_random_segment(bounds);
    }
    return output;
}

std::vector<Segment> lab::gen_unequal_segments(Rect bounds, int count, int seed)
{
    srand(seed);
    const int MAGNET_COUNT = 4;
    std::vector<Rect> magnets(MAGNET_COUNT);
    const double MAGNET_HALFSIZE_MIN = std::min(bounds.ymax - bounds.ymin, bounds.xmax - bounds.xmin) / 5.0;
    const double MAGNET_HALFSIZE_MAX = std::min(bounds.ymax - bounds.ymin, bounds.xmax - bounds.xmin) / 3.0;
    for (int i = 0; i < MAGNET_COUNT; i++) {
        Point mp = (Point) {
            .x = gen_random_double(bounds.xmin, bounds.xmax),
            .y = gen_random_double(bounds.ymin, bounds.ymax),
        };
        Rect magnet = (Rect){
            .xmin = mp.x - gen_random_double(MAGNET_HALFSIZE_MIN, MAGNET_HALFSIZE_MAX),
            .ymin = mp.y - gen_random_double(MAGNET_HALFSIZE_MIN, MAGNET_HALFSIZE_MAX),
            .xmax = mp.x + gen_random_double(MAGNET_HALFSIZE_MIN, MAGNET_HALFSIZE_MAX),
            .ymax = mp.y + gen_random_double(MAGNET_HALFSIZE_MIN, MAGNET_HALFSIZE_MAX),
        };
        if (magnet.xmin < bounds.xmin) magnet.xmin = bounds.xmin; 
        if (magnet.ymin < bounds.ymin) magnet.ymin = bounds.ymin;
        if (magnet.xmax > bounds.xmax) magnet.xmax = bounds.xmax; 
        if (magnet.ymax > bounds.ymax) magnet.ymax = bounds.ymax;
        magnets[i] = magnet;
    }

    std::vector<Segment> output(count);
    for (int i = 0; i < count; i++) {
        int magnet_index = gen_random_int(0, 5);
        if (magnet_index < MAGNET_COUNT) {
            output[i] = gen_random_segment(magnets[magnet_index]);
        } else {
            output[i] = gen_random_segment(bounds);
        }
    }
    return output;
}

std::vector<Rect> lab::dumb_delimit(Rect r, int cnt)
{
    std::vector<Rect> output;
    for (int i = 0; i < cnt; i++) {
        double xmin = r.xmin + i * (r.xmax - r.xmin) / cnt;
        double xmax = r.xmin + (i + 1) * (r.xmax - r.xmin) / cnt;
        if (i != 0) {
            xmin = std::nextafter(xmin, INFINITY); // Prevent dupe intersections
        }
        for (int j = 0; j < cnt; j++) {
            double ymin = r.ymin + j * (r.ymax - r.ymin) / cnt;
            double ymax = r.ymin + (j + 1) * (r.ymax - r.ymin) / cnt;
            if (j != 0) {
                ymin = std::nextafter(ymin, INFINITY); // Prevent dupe intersections
            }
            Rect rect = (Rect) {
                .xmin = xmin,
                .ymin = ymin,
                .xmax = xmax,
                .ymax = ymax
            };
            output.push_back(rect);
        }
    }
    return output;
}

double segment_len(Segment s) {
    return (s.x2 - s.x1) * (s.x2 - s.x1) + (s.y2 - s.y1) * (s.y2 - s.y1);
}

std::vector<std::size_t> sort_segments_by_len(const std::vector<Segment> &v) {

  // initialize original index locations
  std::vector<std::size_t> idx(v.size());
  std::iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  // using std::stable_sort instead of std::sort
  // to avoid unnecessary index re-orderings
  // when v contains elements of equal values 
  stable_sort(idx.begin(), idx.end(),
       [&v](std::size_t i1, std::size_t i2) {return segment_len(v[i1]) < segment_len(v[i2]); });

  return idx;
}
/*
SmartDelimitResult smart_delimit(const std::vector<Segment>& segments)
{
    if (segments.size() == 0) return {};
    std::vector<std::size_t> indexes = sort_segments_by_len(segments);
    SmartDelimitResult output;
    output.rects.push_back(rect_from_segment(segments[indexes[0]]));
    for (int i_ = 1; i_ < indexes.size(); i_++) {
        std::size_t i = indexes[i_];
        Rect rect = rect_from_segment(segments[i]);
        
        int index_intersect = -1;
        bool skip = false;
        for (int j = 0; j < output.rects.size(); j++) {
            bool intersect = are_rects_intersecting(rect, output.rects[j]);
            if (intersect) {
                if (index_intersect == -1) {
                    index_intersect = j;
                } else {
                    skip = true;
                    break;
                }
            }
        }
        if (skip) {
            output.remaining_indexes.push_back(i);
        } else if (index_intersect == -1) {
            output.rects.push_back(rect);
        } else {
            // Expanded may still overlap with some other.
            Rect expanded = lab::expand(output.rects[index_intersect], rect);
            for (int j = 0; j < output.rects.size(); j++) {
                if (j == index_intersect) continue;
                if (are_rects_intersecting(expanded, output.rects[j])) {
                    skip = true;
                    break;
                }
            }
            if (skip) {
                output.remaining_indexes.push_back(i);
            } else {
                output.rects[index_intersect] = expanded;
            }
        }
    }
    return output;
}*/

std::vector<Rect> lab::smarter_delimit(const std::vector<Segment>& segments, Rect rect, int cnt) {
    cnt *= cnt; // that's how dumb_delimit gets it's count :)

    struct SmarterDelimitRect {
        std::vector<int> indexes;
        Rect rect;
        bool vertical;
    };

    std::vector<int> indexes_first(segments.size());
    std::iota(indexes_first.begin(), indexes_first.end(), 0);

    std::vector<SmarterDelimitRect> output = { SmarterDelimitRect{indexes_first, rect, (rect.xmax - rect.xmin) < (rect.ymax - rect.ymin)} };
    output.reserve(cnt);

    while (output.size() < cnt) {
        int i_max = 0;
        for (int i = 1; i < output.size(); i++) {
            if (output[i].indexes.size() > output[i_max].indexes.size()) {
                i_max = i;
            }
        }

        SmarterDelimitRect sdr_to_remove = output[i_max];
        bool vertical = (sdr_to_remove.rect.xmax - sdr_to_remove.rect.xmin) < (sdr_to_remove.rect.ymax - sdr_to_remove.rect.ymin);
        SmarterDelimitRect sdr1 = {.rect = sdr_to_remove.rect, .vertical = vertical};
        SmarterDelimitRect sdr2 = {.rect = sdr_to_remove.rect, .vertical = vertical};
        if (vertical) {
            double y_sep = (sdr_to_remove.rect.ymax - sdr_to_remove.rect.ymin) / 2.0 + sdr_to_remove.rect.ymin;
            sdr1.rect.ymax = y_sep;
            sdr2.rect.ymin = std::nextafter(y_sep, INFINITY); // Prevent dupe intersections
        } else {
            double x_sep = (sdr_to_remove.rect.xmax - sdr_to_remove.rect.xmin) / 2.0 + sdr_to_remove.rect.xmin;
            sdr1.rect.xmax = x_sep;
            sdr2.rect.xmin = std::nextafter(x_sep, INFINITY); // Prevent dupe intersections
        }
        sdr1.indexes.reserve(sdr_to_remove.indexes.size());
        sdr2.indexes.reserve(sdr_to_remove.indexes.size());
        for (int index : sdr_to_remove.indexes) {
            Segment s = segments[index];
            Point p1 = {s.x1, s.y1};
            Point p2 = {s.x2, s.y2};
            if (are_point_and_rect_intersecting(p1, sdr1.rect) || are_point_and_rect_intersecting(p2, sdr1.rect)) {
                sdr1.indexes.push_back(index);
            }
            if (are_point_and_rect_intersecting(p1, sdr2.rect) || are_point_and_rect_intersecting(p2, sdr2.rect)) {
                sdr2.indexes.push_back(index);
            }
        }
        output[i_max] = sdr1; // reuse so I don't need to erase in a vector
        output.push_back(sdr2);
    }

    std::vector<Rect> output_rects(output.size());
    for (int i = 0; i < output.size(); i++) {
        output_rects[i] = output[i].rect;
    }
    return output_rects;
}

static std::vector<Point> calc_intersections_delimited(const std::vector<Segment> &segments, Rect rect)
{
    std::vector<Segment> segments_clipped;
    segments_clipped.reserve(segments.size());
    for (auto s : segments) {
        auto clipped = lab::liang_barsky_clipper(s, rect);
        if (clipped) {
            segments_clipped.push_back(*clipped);
        }
    }

    std::vector<Point> output;
    for (int i = 0; i < segments_clipped.size(); i++) {
        for (int j = i + 1; j < segments_clipped.size(); j++) {
            auto res = lab::intersection(segments_clipped[i], segments_clipped[j]);
            if (res) {
                output.push_back(*res);
            }
        }
    }
    return output;
}

std::vector<Point> lab::calc_dumbest_intersections(const std::vector<Segment> &segments)
{
    std::vector<Point> output;
    for (int i = 0; i < segments.size(); i++) {
        for (int j = i + 1; j < segments.size(); j++) {
            auto res = lab::intersection(segments[i], segments[j]);
            if (res) {
                output.push_back(*res);
            }
        }
    }
    // printf("!");
    return output;
}

std::vector<Point> lab::calc_dumb_intersections(const std::vector<Segment> &segments, std::vector<Rect> rects, int start_index, int end_index)
{
    std::vector<Point> output;
    for (int i = start_index; i < end_index; i++) {
        std::vector<Point> buf = calc_intersections_delimited(segments, rects[i]);
        for (auto point : buf) {
            output.push_back(point);
        }
    }
    // printf("-");
    return output;
}
/*
// TODO: SLOW AND INCORRECT LOL
std::vector<Point> calc_smart_intersections_UNUSED(const std::vector<Segment>& segments, SmartDelimitResult sdr)
{
    std::vector<Point> output;
    for (auto rect : sdr.rects) {
        std::vector<Point> buf = calc_intersections_delimited(segments, rect);
        for (auto point : buf) {
            output.push_back(point);
        }
    }
    for (int i_ = 0; i_ < sdr.remaining_indexes.size(); i_++) {
        int i = sdr.remaining_indexes[i_];
        for (int j_ = i_ + 1; j_ < sdr.remaining_indexes.size(); j_++) {
            int j = sdr.remaining_indexes[j_];
            auto res = lab::intersection(segments[i], segments[j]);
            if (res) {
                output.push_back(*res);
            }
        }
    }
    // printf("~");
    return output;
}*/

std::vector<Point> lab::calc_smarter_intersections(const std::vector<Segment>& segments, std::vector<Rect> rects, int start_index, int end_index) {
    std::vector<Point> output;
    for (int i = start_index; i < end_index; i++) {
        std::vector<Point> buf = calc_intersections_delimited(segments, rects[i]);
        for (auto point : buf) {
            output.push_back(point);
        }
    }
    // printf("-");
    return output;
}

Result run_and_log(std::function<Result()> calc) {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    Result output = calc();
    clock_gettime(CLOCK_MONOTONIC, &end);

    uint64_t diff_ns = (end.tv_sec - start.tv_sec) * 1000000000LL 
                      + (end.tv_nsec - start.tv_nsec);
    output.ns = diff_ns;
    printf("%s took %.4f. Result has %i points.\n", output.label.c_str(), (double)output.ns / 1000000000.0, (int)output.intersections.size());
    return output;
}

std::vector<Result> lab::calc_all_and_log_results(Rect r, const std::vector<Segment> &segments)
{
    printf("-=-=-=-=-=-=-=-=-=-=-\n");
    Result res1 = run_and_log([&segments]{
        std::vector<Point> points = calc_dumbest_intersections(segments);
        return Result{
            .label = "dumbest",
            .intersections = points,
            .rects = {}
        };
    });
    Result res2 = run_and_log([r, &segments]{
        Result result;
        result.label = "with_dumb_delimit";
        result.rects = dumb_delimit(r, 5);
        result.intersections = calc_dumb_intersections(segments, result.rects, 0, result.rects.size());
        return result;
    });
    Result res3 = run_and_log([r, &segments]{
        Result result;
        result.label = "smarter_delimit";
        result.rects = smarter_delimit(segments, r, 5);
        result.intersections = calc_smarter_intersections(segments, result.rects, 0, result.rects.size());
        return result;
    });
    Result res4 = run_and_log([r, &segments]{
        Result result;
        result.label = "with_dumb_delimit[2thread]";
        result.rects = dumb_delimit(r, 5);
        std::future<std::vector<Point>> f1 = std::async(std::launch::async, [&]{ return calc_dumb_intersections(segments, result.rects, 0, result.rects.size() / 2); });
        std::future<std::vector<Point>> f2 = std::async(std::launch::async, [&]{ return calc_dumb_intersections(segments, result.rects, result.rects.size() / 2, result.rects.size()); });
        result.intersections = f1.get();
        std::vector<Point> f2points = f2.get();
        for (int i = 0; i < f2points.size(); i++) {
            result.intersections.push_back(f2points[i]);
        }
        return result;
    });
    Result res5 = run_and_log([r, &segments]{
        Result result;
        result.label = "smarter_delimit[2thread]";
        result.rects = smarter_delimit(segments, r, 5);
        std::future<std::vector<Point>> f1 = std::async(std::launch::async, [&]{ return calc_smarter_intersections(segments, result.rects, 0, result.rects.size() / 2); });
        std::future<std::vector<Point>> f2 = std::async(std::launch::async, [&]{ return calc_smarter_intersections(segments, result.rects, result.rects.size() / 2, result.rects.size()); });
        result.intersections = f1.get();
        std::vector<Point> f2points = f2.get();
        for (int i = 0; i < f2points.size(); i++) {
            result.intersections.push_back(f2points[i]);
        }
        return result;
    });
    Result res6 = run_and_log([r, &segments]{
        Result result;
        result.label = "with_dumb_delimit[2thread, 225delimits]";
        result.rects = dumb_delimit(r, 15);
        std::future<std::vector<Point>> f1 = std::async(std::launch::async, [&]{ return calc_dumb_intersections(segments, result.rects, 0, result.rects.size() / 2); });
        std::future<std::vector<Point>> f2 = std::async(std::launch::async, [&]{ return calc_dumb_intersections(segments, result.rects, result.rects.size() / 2, result.rects.size()); });
        result.intersections = f1.get();
        std::vector<Point> f2points = f2.get();
        for (int i = 0; i < f2points.size(); i++) {
            result.intersections.push_back(f2points[i]);
        }
        return result;
    });
    Result res7 = run_and_log([r, &segments]{
        Result result;
        result.label = "smarter_delimit[2thread, 225delimits]";
        result.rects = smarter_delimit(segments, r, 15);
        std::future<std::vector<Point>> f1 = std::async(std::launch::async, [&]{ return calc_smarter_intersections(segments, result.rects, 0, result.rects.size() / 2); });
        std::future<std::vector<Point>> f2 = std::async(std::launch::async, [&]{ return calc_smarter_intersections(segments, result.rects, result.rects.size() / 2, result.rects.size()); });
        result.intersections = f1.get();
        std::vector<Point> f2points = f2.get();
        for (int i = 0; i < f2points.size(); i++) {
            result.intersections.push_back(f2points[i]);
        }
        return result;
    });
    printf("-=-=-=-=-=-=-=-=-=-=-\n");

    return {res1, res2, res3, res4, res5, res6, res7};
}
