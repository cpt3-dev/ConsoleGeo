#pragma once

#include "conic.hpp"
#include <vector>

namespace ConsoleGeo {

struct Line {
    double L, M, N;

    Line() : L(0.0), M(0.0), N(0.0) {}

    Line(double l, double m, double n) : L(l), M(m), N(n) {}

    static Line fromTwoPoints(const Point& p1, const Point& p2) {
        double dx = p2.x - p1.x;
        double dy = p2.y - p1.y;
        double L = -dy;
        double M = dx;
        double N = -(L * p1.x + M * p1.y);
        return Line(L, M, N);
    }

    static Line fromPointAndDirection(const Point& p, double dx, double dy) {
        double L = -dy;
        double M = dx;
        double N = -(L * p.x + M * p.y);
        return Line(L, M, N);
    }

    double evaluateAt(const Point& p) const {
        return L * p.x + M * p.y + N;
    }

    bool isVertical() const {
        constexpr double epsilon = 1e-12;
        return std::abs(M) < epsilon;
    }

    bool isHorizontal() const {
        constexpr double epsilon = 1e-12;
        return std::abs(L) < epsilon;
    }

    double slope() const {
        if (isVertical()) {
            return std::numeric_limits<double>::infinity();
        }
        return -L / M;
    }
};

enum class IntersectionResult {
    Intersecting,
    Tangent,
    Separate
};

struct IntersectionInfo {
    IntersectionResult result;
    std::vector<Point> points;
};

class IntersectionSolver {
public:
    IntersectionInfo solve(const Conic& conic, const Line& line) const;

private:
    IntersectionInfo solveNonVertical(const Conic& conic, const Line& line) const;
    IntersectionInfo solveVertical(const Conic& conic, const Line& line) const;
};

} // namespace ConsoleGeo