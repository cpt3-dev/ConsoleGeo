#include "canvas_draw.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace ConsoleGeo {

namespace {
    constexpr int CONIC_SAMPLES = 300;
    constexpr double EPSILON = 1e-12;

    double computeNiceStep(double range) {
        if (range <= 0.0 || !std::isfinite(range)) return 1.0;
        double rawStep = range / 10.0;
        if (rawStep < 1e-10 || !std::isfinite(rawStep)) return 1.0;
        double exponent = std::floor(std::log10(rawStep));
        double mantissa = rawStep / std::pow(10.0, exponent);
        double nice;
        if (mantissa < 1.5) nice = 1.0;
        else if (mantissa < 3.5) nice = 2.0;
        else if (mantissa < 7.5) nice = 5.0;
        else nice = 10.0;
        return nice * std::pow(10.0, exponent);
    }

    bool inPixelRange(int val, int maxVal) {
        return val >= 0 && val < maxVal;
    }

    int safeClamp(int val, int minVal, int maxVal) {
        if (val < minVal) return minVal;
        if (val > maxVal) return maxVal;
        return val;
    }
}

CoordinateCanvas::CoordinateCanvas(double xMin, double xMax, double yMin, double yMax,
                                   int canvasPixelW, int canvasPixelH)
    : xMin_(xMin), xMax_(xMax), yMin_(yMin), yMax_(yMax)
    , pixelWidth_(canvasPixelW > 1 ? canvasPixelW : 2)
    , pixelHeight_(canvasPixelH > 1 ? canvasPixelH : 2)
{
    if (!std::isfinite(xMin_) || !std::isfinite(xMax_)) { xMin_ = -10.0; xMax_ = 10.0; }
    if (!std::isfinite(yMin_) || !std::isfinite(yMax_)) { yMin_ = -10.0; yMax_ = 10.0; }
    if (xMax_ <= xMin_) xMax_ = xMin_ + 10.0;
    if (yMax_ <= yMin_) yMax_ = yMin_ + 10.0;
}

int CoordinateCanvas::toPixelX(double x) const {
    double range = xMax_ - xMin_;
    if (range <= 0.0 || !std::isfinite(range)) return 0;
    if (!std::isfinite(x)) return 0;
    double ratio = (x - xMin_) / range;
    if (ratio < 0.0) ratio = 0.0;
    if (ratio > 1.0) ratio = 1.0;
    int px = static_cast<int>(std::round(ratio * (pixelWidth_ - 1)));
    return safeClamp(px, 0, pixelWidth_ - 1);
}

int CoordinateCanvas::toPixelY(double y) const {
    double range = yMax_ - yMin_;
    if (range <= 0.0 || !std::isfinite(range)) return 0;
    if (!std::isfinite(y)) return 0;
    double ratio = (y - yMin_) / range;
    if (ratio < 0.0) ratio = 0.0;
    if (ratio > 1.0) ratio = 1.0;
    int py = pixelHeight_ - 1 - static_cast<int>(std::round(ratio * (pixelHeight_ - 1)));
    return safeClamp(py, 0, pixelHeight_ - 1);
}

double CoordinateCanvas::toLogicalX(int px) const {
    if (pixelWidth_ <= 1) return xMin_;
    double ratio = static_cast<double>(safeClamp(px, 0, pixelWidth_ - 1)) / (pixelWidth_ - 1);
    return xMin_ + ratio * (xMax_ - xMin_);
}

double CoordinateCanvas::toLogicalY(int py) const {
    if (pixelHeight_ <= 1) return yMin_;
    double ratio = static_cast<double>(pixelHeight_ - 1 - safeClamp(py, 0, pixelHeight_ - 1)) / (pixelHeight_ - 1);
    return yMin_ + ratio * (yMax_ - yMin_);
}

int CoordinateCanvas::getPixelWidth() const { return pixelWidth_; }
int CoordinateCanvas::getPixelHeight() const { return pixelHeight_; }
double CoordinateCanvas::getXMin() const { return xMin_; }
double CoordinateCanvas::getXMax() const { return xMax_; }
double CoordinateCanvas::getYMin() const { return yMin_; }
double CoordinateCanvas::getYMax() const { return yMax_; }

void CoordinateCanvas::drawAxes() {
    ftxui::Canvas dummy(1, 1);
    drawAxesOn(dummy);
}

void CoordinateCanvas::drawGrid() {
    ftxui::Canvas dummy(1, 1);
    drawGridOn(dummy);
}

void CoordinateCanvas::drawConic(const Conic& conic) {
    ftxui::Canvas dummy(1, 1);
    drawConicOn(dummy, conic);
}

void CoordinateCanvas::drawPoints(const std::vector<Point>& points) {
    ftxui::Canvas dummy(1, 1);
    drawPointsOn(dummy, points);
}

void CoordinateCanvas::drawLabel(const std::string& text, double x, double y) {
    ftxui::Canvas dummy(1, 1);
    drawLabelOn(dummy, text, x, y);
}

void CoordinateCanvas::drawAllOn(ftxui::Canvas& c, const Conic& conic,
                                 const std::vector<Point>& points,
                                 bool mouseValid, double mouseX, double mouseY) {
    pixelWidth_ = c.width();
    pixelHeight_ = c.height();
    if (pixelWidth_ < 2) pixelWidth_ = 2;
    if (pixelHeight_ < 2) pixelHeight_ = 2;

    drawGridOn(c);
    drawAxesOn(c);
    drawConicOn(c, conic);
    drawPointsOn(c, points);
    if (mouseValid) {
        drawLabelOn(c, "X", mouseX, mouseY);
    }
}

void CoordinateCanvas::drawAxesOn(ftxui::Canvas& c) {
    int originPx = toPixelX(0.0);
    int originPy = toPixelY(0.0);
    int leftPx   = toPixelX(xMin_);
    int rightPx  = toPixelX(xMax_);
    int topPy    = toPixelY(yMax_);
    int bottomPy = toPixelY(yMin_);

    if (inPixelRange(originPy, pixelHeight_)) {
        c.DrawBlockLine(leftPx, originPy, rightPx, originPy);
        int arrowX = rightPx - (rightPx % 2);
        if (arrowX >= 2) {
            c.DrawText(arrowX, originPy, ">");
        }
    }

    if (inPixelRange(originPx, pixelWidth_)) {
        c.DrawBlockLine(originPx, bottomPy, originPx, topPy);
        int arrowY = topPy - (topPy % 4);
        if (arrowY >= 0 && arrowY < pixelHeight_) {
            c.DrawText(originPx, arrowY, "^");
        }
    }
}

void CoordinateCanvas::drawGridOn(ftxui::Canvas& c) {
    double xRange = xMax_ - xMin_;
    double yRange = yMax_ - yMin_;
    if (xRange <= 0.0 || !std::isfinite(xRange)) xRange = 1.0;
    if (yRange <= 0.0 || !std::isfinite(yRange)) yRange = 1.0;

    double xStep = computeNiceStep(xRange);
    double yStep = computeNiceStep(yRange);

    int topPy    = toPixelY(yMax_);
    int bottomPy = toPixelY(yMin_);
    int leftPx   = toPixelX(xMin_);
    int rightPx  = toPixelX(xMax_);

    double xStart = std::ceil(xMin_ / xStep) * xStep;
    for (double x = xStart; x <= xMax_ + EPSILON; x += xStep) {
        if (!std::isfinite(x)) break;
        int px = toPixelX(x);
        if (inPixelRange(px, pixelWidth_) &&
            inPixelRange(topPy, pixelHeight_) && inPixelRange(bottomPy, pixelHeight_)) {
            c.DrawBlockLine(px, topPy, px, bottomPy);
        }
    }

    double yStart = std::ceil(yMin_ / yStep) * yStep;
    for (double y = yStart; y <= yMax_ + EPSILON; y += yStep) {
        if (!std::isfinite(y)) break;
        int py = toPixelY(y);
        if (inPixelRange(py, pixelHeight_) &&
            inPixelRange(leftPx, pixelWidth_) && inPixelRange(rightPx, pixelWidth_)) {
            c.DrawBlockLine(leftPx, py, rightPx, py);
        }
    }
}

void CoordinateCanvas::drawConicOn(ftxui::Canvas& c, const Conic& conic) {
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

                int px = toPixelX(x);
                if (inPixelRange(px, pixelWidth_)) {
                    int py1 = toPixelY(y1);
                    if (inPixelRange(py1, pixelHeight_)) c.DrawPoint(px, py1, true);
                    if (std::abs(y1 - y2) > EPSILON) {
                        int py2 = toPixelY(y2);
                        if (inPixelRange(py2, pixelHeight_)) c.DrawPoint(px, py2, true);
                    }
                }
            }
        } else if (std::abs(bY) > EPSILON) {
            double y = -cY / bY;
            if (!std::isfinite(y)) continue;
            int px = toPixelX(x);
            int py = toPixelY(y);
            if (inPixelRange(px, pixelWidth_) && inPixelRange(py, pixelHeight_))
                c.DrawPoint(px, py, true);
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

                int py = toPixelY(y);
                if (inPixelRange(py, pixelHeight_)) {
                    int px1 = toPixelX(x1);
                    if (inPixelRange(px1, pixelWidth_)) c.DrawPoint(px1, py, true);
                    if (std::abs(x1 - x2) > EPSILON) {
                        int px2 = toPixelX(x2);
                        if (inPixelRange(px2, pixelWidth_)) c.DrawPoint(px2, py, true);
                    }
                }
            }
        } else if (std::abs(bX) > EPSILON) {
            double x = -cX / bX;
            if (!std::isfinite(x)) continue;
            int px = toPixelX(x);
            int py = toPixelY(y);
            if (inPixelRange(px, pixelWidth_) && inPixelRange(py, pixelHeight_))
                c.DrawPoint(px, py, true);
        }
    }
}

void CoordinateCanvas::drawPointsOn(ftxui::Canvas& c, const std::vector<Point>& points) {
    for (const auto& pt : points) {
        if (!std::isfinite(pt.x) || !std::isfinite(pt.y)) continue;
        int cx = toPixelX(pt.x);
        int cy = toPixelY(pt.y);

        for (int dx = -2; dx <= 2; ++dx) {
            int px = cx + dx;
            if (inPixelRange(px, pixelWidth_) && inPixelRange(cy, pixelHeight_))
                c.DrawPoint(px, cy, true);
        }
        for (int dy = -2; dy <= 2; ++dy) {
            int py = cy + dy;
            if (inPixelRange(cx, pixelWidth_) && inPixelRange(py, pixelHeight_))
                c.DrawPoint(cx, py, true);
        }
    }
}

void CoordinateCanvas::drawLabelOn(ftxui::Canvas& c, const std::string& text, double x, double y) {
    if (!std::isfinite(x) || !std::isfinite(y)) return;
    int px = toPixelX(x);
    int py = toPixelY(y);
    if (inPixelRange(px, pixelWidth_) && inPixelRange(py, pixelHeight_)) {
        c.DrawText(px, py, text);
    }
}

} // namespace ConsoleGeo