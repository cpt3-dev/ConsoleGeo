#include "canvas_draw.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <cmath>
#include <algorithm>

namespace ConsoleGeo {

namespace {
    constexpr int CONIC_SAMPLES = 150;
    constexpr double EPSILON = 1e-12;

    double computeNiceStep(double range) {
        if (range <= 0.0 || !std::isfinite(range)) return 1.0;
        double rawStep = range / 8.0;
        if (rawStep < 1e-10 || !std::isfinite(rawStep)) return 1.0;
        double exponent = std::floor(std::log10(rawStep));
        double mantissa = rawStep / std::pow(10.0, exponent);
        if (mantissa < 1.5)      return 1.0  * std::pow(10.0, exponent);
        else if (mantissa < 3.5) return 2.0  * std::pow(10.0, exponent);
        else if (mantissa < 7.5) return 5.0  * std::pow(10.0, exponent);
        else                     return 10.0 * std::pow(10.0, exponent);
    }
}

CoordinateCanvas::CoordinateCanvas(double xMin, double xMax, double yMin, double yMax,
                                   int charW, int charH)
    : xMin_(xMin), xMax_(xMax), yMin_(yMin), yMax_(yMax)
    , charW_(charW > 2 ? charW : 10)
    , charH_(charH > 2 ? charH : 10)
{
    if (!std::isfinite(xMin_) || !std::isfinite(xMax_)) { xMin_ = -10.0; xMax_ = 10.0; }
    if (!std::isfinite(yMin_) || !std::isfinite(yMax_)) { yMin_ = -10.0; yMax_ = 10.0; }
    if (xMax_ <= xMin_) xMax_ = xMin_ + 10.0;
    if (yMax_ <= yMin_) yMax_ = yMin_ + 10.0;
}

int CoordinateCanvas::toCharX(double x) const {
    if (charW_ <= 1) return 0;
    double ratio = (x - xMin_) / (xMax_ - xMin_);
    if (!std::isfinite(ratio)) ratio = 0.0;
    ratio = std::max(0.0, std::min(1.0, ratio));
    return static_cast<int>(std::round(ratio * (charW_ - 1)));
}

int CoordinateCanvas::toCharY(double y) const {
    if (charH_ <= 1) return 0;
    double ratio = (y - yMin_) / (yMax_ - yMin_);
    if (!std::isfinite(ratio)) ratio = 0.0;
    ratio = std::max(0.0, std::min(1.0, ratio));
    return charH_ - 1 - static_cast<int>(std::round(ratio * (charH_ - 1)));
}

void CoordinateCanvas::setChar(int x, int y, char ch) {
    if (x < 0 || x >= charW_ || y < 0 || y >= charH_) return;
    grid_[y][x] = ch;
}

void CoordinateCanvas::drawGridLines() {
    double xStep = computeNiceStep(xMax_ - xMin_);
    double yStep = computeNiceStep(yMax_ - yMin_);

    double xStart = std::ceil(xMin_ / xStep) * xStep;
    for (double x = xStart; x <= xMax_ + EPSILON; x += xStep) {
        if (!std::isfinite(x)) break;
        int bx = toCharX(x);
        if (bx < 0 || bx >= charW_) continue;
        for (int y = 0; y < charH_; ++y) {
            if (grid_[y][bx] == ' ') grid_[y][bx] = ':';
        }
    }

    double yStart = std::ceil(yMin_ / yStep) * yStep;
    for (double y = yStart; y <= yMax_ + EPSILON; y += yStep) {
        if (!std::isfinite(y)) break;
        int by = toCharY(y);
        if (by < 0 || by >= charH_) continue;
        for (int x = 0; x < charW_; ++x) {
            if (grid_[by][x] == ' ') grid_[by][x] = ':';
        }
    }
}

void CoordinateCanvas::drawAxes() {
    int ox = toCharX(0.0);
    int oy = toCharY(0.0);

    if (oy >= 0 && oy < charH_) {
        for (int x = 0; x < charW_; ++x) {
            if (grid_[oy][x] == ' ' || grid_[oy][x] == ':')
                grid_[oy][x] = '-';
        }
        if (ox >= 0 && ox < charW_) grid_[oy][ox] = '+';
        if (charW_ >= 2) grid_[oy][charW_ - 1] = '>';
    }

    if (ox >= 0 && ox < charW_) {
        for (int y = 0; y < charH_; ++y) {
            if (grid_[y][ox] == ' ' || grid_[y][ox] == ':')
                grid_[y][ox] = '|';
        }
        if (oy >= 0 && oy < charH_) grid_[oy][ox] = '+';
        if (charH_ >= 2) grid_[0][ox] = '^';
    }
}

void CoordinateCanvas::drawConic(const Conic& conic) {
    double A = conic.A;
    double B = conic.B;
    double C = conic.C;
    double D = conic.D;
    double E = conic.E;
    double F = conic.F;

    for (int i = 0; i < CONIC_SAMPLES; ++i) {
        double x = xMin_ + (static_cast<double>(i) / (CONIC_SAMPLES - 1)) * (xMax_ - xMin_);
        if (!std::isfinite(x)) continue;

        double aY = C;
        double bY = B * x + E;
        double cY = A * x * x + D * x + F;

        if (std::abs(aY) > EPSILON) {
            double disc = bY * bY - 4.0 * aY * cY;
            if (disc >= 0.0) {
                double sqrtDisc = std::sqrt(disc);
                double y1 = (-bY + sqrtDisc) / (2.0 * aY);
                double y2 = (-bY - sqrtDisc) / (2.0 * aY);
                int bx = toCharX(x);
                if (bx < 0 || bx >= charW_) continue;
                for (double y : {y1, y2}) {
                    int by = toCharY(y);
                    if (by >= 0 && by < charH_) setChar(bx, by, '*');
                }
            }
        } else if (std::abs(bY) > EPSILON) {
            double y = -cY / bY;
            if (!std::isfinite(y)) continue;
            int bx = toCharX(x);
            int by = toCharY(y);
            if (bx >= 0 && bx < charW_ && by >= 0 && by < charH_)
                setChar(bx, by, '*');
        }
    }

    for (int i = 0; i < CONIC_SAMPLES; ++i) {
        double y = yMin_ + (static_cast<double>(i) / (CONIC_SAMPLES - 1)) * (yMax_ - yMin_);
        if (!std::isfinite(y)) continue;

        double aX = A;
        double bX = B * y + D;
        double cX = C * y * y + E * y + F;

        if (std::abs(aX) > EPSILON) {
            double disc = bX * bX - 4.0 * aX * cX;
            if (disc >= 0.0) {
                double sqrtDisc = std::sqrt(disc);
                double x1 = (-bX + sqrtDisc) / (2.0 * aX);
                double x2 = (-bX - sqrtDisc) / (2.0 * aX);
                int by = toCharY(y);
                if (by < 0 || by >= charH_) continue;
                for (double x : {x1, x2}) {
                    int bx = toCharX(x);
                    if (bx >= 0 && bx < charW_) setChar(bx, by, '*');
                }
            }
        } else if (std::abs(bX) > EPSILON) {
            double x = -cX / bX;
            if (!std::isfinite(x)) continue;
            int bx = toCharX(x);
            int by = toCharY(y);
            if (bx >= 0 && bx < charW_ && by >= 0 && by < charH_)
                setChar(bx, by, '*');
        }
    }
}

void CoordinateCanvas::drawPoints(const std::vector<Point>& points) {
    for (const auto& pt : points) {
        if (!std::isfinite(pt.x) || !std::isfinite(pt.y)) continue;
        int cx = toCharX(pt.x);
        int cy = toCharY(pt.y);
        for (int dx = -1; dx <= 1; ++dx) {
            int px = cx + dx;
            for (int dy = -1; dy <= 1; ++dy) {
                int py = cy + dy;
                if (px >= 0 && px < charW_ && py >= 0 && py < charH_)
                    setChar(px, py, '#');
            }
        }
    }
}

void CoordinateCanvas::drawMouse(bool valid, double x, double y) {
    if (!valid || !std::isfinite(x) || !std::isfinite(y)) return;
    int bx = toCharX(x);
    int by = toCharY(y);
    if (bx >= 0 && bx < charW_ && by >= 0 && by < charH_)
        setChar(bx, by, '@');
}

ftxui::Element CoordinateCanvas::render(const Conic& conic,
                                        const std::vector<Point>& points,
                                        bool mouseValid, double mouseX, double mouseY) {
    // Hard limits to prevent excessive memory allocation
    if (charW_ < 2) charW_ = 10;
    if (charW_ > 300) charW_ = 300;
    if (charH_ < 2) charH_ = 10;
    if (charH_ > 100) charH_ = 100;

    grid_.assign(charH_, std::string(charW_, ' '));

    drawGridLines();
    drawAxes();
    drawConic(conic);
    drawPoints(points);
    drawMouse(mouseValid, mouseX, mouseY);

    // Join all rows into a single string with newlines
    std::string fullText;
    fullText.reserve(charH_ * (charW_ + 1));
    for (int y = 0; y < charH_; ++y) {
        if (y > 0) fullText += '\n';
        fullText += grid_[y];
    }

    return ftxui::text(fullText) | ftxui::border;
}

} // namespace ConsoleGeo