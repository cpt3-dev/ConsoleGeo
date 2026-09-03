#pragma once

#include <cmath>
#include <numbers>

namespace ConsoleGeo {

struct Point {
    double x;
    double y;

    Point() : x(0.0), y(0.0) {}
    Point(double x_, double y_) : x(x_), y(y_) {}

    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }

    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }

    Point operator*(double s) const {
        return Point(x * s, y * s);
    }

    double dot(const Point& other) const {
        return x * other.x + y * other.y;
    }

    double lengthSq() const {
        return x * x + y * y;
    }

    double length() const {
        return std::sqrt(lengthSq());
    }

    double distanceTo(const Point& other) const {
        return (*this - other).length();
    }
};

enum class ConicType {
    Circle,
    Ellipse,
    Parabola,
    Hyperbola,
    Unknown
};

enum class ParabolaDirection {
    Right,
    Up,
    Left,
    Down
};

class Conic {
public:
    double A, B, C, D, E, F;
    ConicType type;

    Conic()
        : A(0.0), B(0.0), C(0.0), D(0.0), E(0.0), F(0.0)
        , type(ConicType::Unknown)
    {}

    Conic(double a, double b, double c, double d, double e, double f, ConicType t = ConicType::Unknown)
        : A(a), B(b), C(c), D(d), E(e), F(f), type(t)
    {}

    double evaluateAt(double x, double y) const {
        return A * x * x + B * x * y + C * y * y + D * x + E * y + F;
    }

    double evaluateAt(const Point& p) const {
        return evaluateAt(p.x, p.y);
    }

    double discriminant() const {
        return B * B - 4.0 * A * C;
    }

    ConicType getType() const;

    ConicType classifyByDiscriminant() const;

    static Conic createCircle(double h, double k, double r);
    static Conic createEllipse(double h, double k, double a, double b, double angle = 0.0);
    static Conic createParabola(double h, double k, double p, ParabolaDirection dir);
    static Conic createHyperbola(double h, double k, double a, double b, double angle = 0.0);
};

ConicType classifyConic(double A, double B, double C, double D, double E, double F);

} // namespace ConsoleGeo