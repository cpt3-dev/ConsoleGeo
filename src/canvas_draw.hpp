#pragma once

#include "conic.hpp"
#include "intersection.hpp"
#include <ftxui/dom/canvas.hpp>
#include <vector>
#include <string>

namespace ConsoleGeo {

class CoordinateCanvas {
public:
    CoordinateCanvas(double xMin, double xMax, double yMin, double yMax,
                     int canvasPixelW, int canvasPixelH);

    void drawAxes();
    void drawGrid();
    void drawConic(const Conic& conic);
    void drawPoints(const std::vector<Point>& points);
    void drawLabel(const std::string& text, double x, double y);

    int toPixelX(double x) const;
    int toPixelY(double y) const;
    double toLogicalX(int px) const;
    double toLogicalY(int py) const;

    int getPixelWidth() const;
    int getPixelHeight() const;

    double getXMin() const;
    double getXMax() const;
    double getYMin() const;
    double getYMax() const;

    void drawAllOn(ftxui::Canvas& c, const Conic& conic,
                   const std::vector<Point>& points,
                   bool mouseValid, double mouseX, double mouseY);

private:
    double xMin_, xMax_, yMin_, yMax_;
    int pixelWidth_, pixelHeight_;

    void drawAxesOn(ftxui::Canvas& c);
    void drawGridOn(ftxui::Canvas& c);
    void drawConicOn(ftxui::Canvas& c, const Conic& conic);
    void drawPointsOn(ftxui::Canvas& c, const std::vector<Point>& points);
    void drawLabelOn(ftxui::Canvas& c, const std::string& text, double x, double y);
};

} // namespace ConsoleGeo