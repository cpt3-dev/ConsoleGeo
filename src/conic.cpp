#include "conic.hpp"
#include <cmath>

namespace ConsoleGeo {

ConicType Conic::getType() const {
    return classifyByDiscriminant();
}

ConicType Conic::classifyByDiscriminant() const {
    return classifyConic(A, B, C, D, E, F);
}

ConicType classifyConic(double A, double B, double C, double D, double E, double F) {
    constexpr double epsilon = 1e-12;

    bool aZero = std::abs(A) < epsilon;
    bool bZero = std::abs(B) < epsilon;
    bool cZero = std::abs(C) < epsilon;

    if (aZero && bZero && cZero) {
        return ConicType::Unknown;
    }

    double disc = B * B - 4.0 * A * C;

    if (disc > epsilon) {
        return ConicType::Hyperbola;
    }

    if (std::abs(disc) <= epsilon) {
        return ConicType::Parabola;
    }

    if (aZero && bZero && cZero) {
        return ConicType::Unknown;
    }

    if (std::abs(A - C) < epsilon && bZero) {
        return ConicType::Circle;
    }

    return ConicType::Ellipse;
}

Conic Conic::createCircle(double h, double k, double r) {
    double A = 1.0;
    double B = 0.0;
    double C = 1.0;
    double D = -2.0 * h;
    double E = -2.0 * k;
    double F = h * h + k * k - r * r;
    return Conic(A, B, C, D, E, F, ConicType::Circle);
}

Conic Conic::createEllipse(double h, double k, double a, double b, double angle) {
    double cosT = std::cos(angle);
    double sinT = std::sin(angle);
    double cos2 = cosT * cosT;
    double sin2 = sinT * sinT;
    double a2 = a * a;
    double b2 = b * b;

    double A = cos2 / a2 + sin2 / b2;
    double B_coef = 2.0 * cosT * sinT * (1.0 / a2 - 1.0 / b2);
    double C = sin2 / a2 + cos2 / b2;

    double D = -2.0 * A * h - B_coef * k;
    double E = -B_coef * h - 2.0 * C * k;
    double F = A * h * h + B_coef * h * k + C * k * k - 1.0;

    return Conic(A, B_coef, C, D, E, F, ConicType::Ellipse);
}

Conic Conic::createParabola(double h, double k, double p, ParabolaDirection dir) {
    double A = 0.0, B = 0.0, C = 0.0, D = 0.0, E = 0.0, F = 0.0;

    switch (dir) {
        case ParabolaDirection::Right:
            C = 1.0;
            D = -4.0 * p;
            E = -2.0 * k;
            F = k * k + 4.0 * p * h;
            break;
        case ParabolaDirection::Left:
            C = 1.0;
            D = 4.0 * p;
            E = -2.0 * k;
            F = k * k - 4.0 * p * h;
            break;
        case ParabolaDirection::Up:
            A = 1.0;
            D = -2.0 * h;
            E = -4.0 * p;
            F = h * h + 4.0 * p * k;
            break;
        case ParabolaDirection::Down:
            A = 1.0;
            D = -2.0 * h;
            E = 4.0 * p;
            F = h * h - 4.0 * p * k;
            break;
    }

    return Conic(A, B, C, D, E, F, ConicType::Parabola);
}

Conic Conic::createHyperbola(double h, double k, double a, double b, double angle) {
    double cosT = std::cos(angle);
    double sinT = std::sin(angle);
    double cos2 = cosT * cosT;
    double sin2 = sinT * sinT;
    double a2 = a * a;
    double b2 = b * b;

    double A = cos2 / a2 - sin2 / b2;
    double B_coef = 2.0 * cosT * sinT * (1.0 / a2 + 1.0 / b2);
    double C = sin2 / a2 - cos2 / b2;

    double D = -2.0 * A * h - B_coef * k;
    double E = -B_coef * h - 2.0 * C * k;
    double F = A * h * h + B_coef * h * k + C * k * k - 1.0;

    return Conic(A, B_coef, C, D, E, F, ConicType::Hyperbola);
}

} // namespace ConsoleGeo