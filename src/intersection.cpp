#include "intersection.hpp"
#include <cmath>
#include <limits>

namespace ConsoleGeo {

IntersectionInfo IntersectionSolver::solve(const Conic& conic, const Line& line) const {
    if (line.isVertical()) {
        return solveVertical(conic, line);
    }
    return solveNonVertical(conic, line);
}

IntersectionInfo IntersectionSolver::solveNonVertical(const Conic& conic, const Line& line) const {
    constexpr double epsilon = 1e-12;

    double L = line.L;
    double M = line.M;
    double N_val = line.N;

    double A = conic.A;
    double B = conic.B;
    double C = conic.C;
    double D = conic.D;
    double E = conic.E;
    double F = conic.F;

    double M2 = M * M;
    double L2 = L * L;
    double N2 = N_val * N_val;

    double a = A * M2 - B * M * L + C * L2;
    double b = -B * M * N_val + 2.0 * C * L * N_val + D * M2 - E * M * L;
    double c = C * N2 - E * M * N_val + F * M2;

    IntersectionInfo info;

    if (std::abs(a) < epsilon) {
        if (std::abs(b) < epsilon) {
            info.result = IntersectionResult::Separate;
            return info;
        }
        double x = -c / b;
        double y = -(L * x + N_val) / M;
        info.result = IntersectionResult::Intersecting;
        info.points.push_back(Point(x, y));
        return info;
    }

    double disc = b * b - 4.0 * a * c;

    if (disc < -epsilon) {
        info.result = IntersectionResult::Separate;
        return info;
    }

    if (std::abs(disc) < epsilon) {
        double x = -b / (2.0 * a);
        double y = -(L * x + N_val) / M;
        info.result = IntersectionResult::Tangent;
        info.points.push_back(Point(x, y));
        return info;
    }

    double sqrtDisc = std::sqrt(disc);
    double x1 = (-b + sqrtDisc) / (2.0 * a);
    double x2 = (-b - sqrtDisc) / (2.0 * a);
    double y1 = -(L * x1 + N_val) / M;
    double y2 = -(L * x2 + N_val) / M;

    info.result = IntersectionResult::Intersecting;
    info.points.push_back(Point(x1, y1));
    info.points.push_back(Point(x2, y2));
    return info;
}

IntersectionInfo IntersectionSolver::solveVertical(const Conic& conic, const Line& line) const {
    constexpr double epsilon = 1e-12;

    double L = line.L;
    double N_val = line.N;

    if (std::abs(L) < epsilon) {
        IntersectionInfo info;
        info.result = IntersectionResult::Separate;
        return info;
    }

    double x0 = -N_val / L;

    double A = conic.A;
    double B = conic.B;
    double C = conic.C;
    double D = conic.D;
    double E = conic.E;
    double F = conic.F;

    double a = C;
    double b = B * x0 + E;
    double c = A * x0 * x0 + D * x0 + F;

    IntersectionInfo info;

    if (std::abs(a) < epsilon) {
        if (std::abs(b) < epsilon) {
            info.result = IntersectionResult::Separate;
            return info;
        }
        double y = -c / b;
        info.result = IntersectionResult::Intersecting;
        info.points.push_back(Point(x0, y));
        return info;
    }

    double disc = b * b - 4.0 * a * c;

    if (disc < -epsilon) {
        info.result = IntersectionResult::Separate;
        return info;
    }

    if (std::abs(disc) < epsilon) {
        double y = -b / (2.0 * a);
        info.result = IntersectionResult::Tangent;
        info.points.push_back(Point(x0, y));
        return info;
    }

    double sqrtDisc = std::sqrt(disc);
    double y1 = (-b + sqrtDisc) / (2.0 * a);
    double y2 = (-b - sqrtDisc) / (2.0 * a);

    info.result = IntersectionResult::Intersecting;
    info.points.push_back(Point(x0, y1));
    info.points.push_back(Point(x0, y2));
    return info;
}

} // namespace ConsoleGeo