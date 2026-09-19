#pragma once

#include "conic.hpp"
#include "intersection.hpp"
#include <ftxui/dom/elements.hpp>
#include <vector>
#include <string>

namespace ConsoleGeo {

class CoordinateCanvas {
public:
    CoordinateCanvas(double xMin, double xMax, double yMin, double yMax,
                     int charW, int charH);

    ftxui::Element render(const Conic& conic,
                          const std::vector<Point>& points,
                          bool mouseValid, double mouseX, double mouseY);

private:
    double xMin_, xMax_, yMin_, yMax_;
    int charW_, charH_;
    std::vector<std::string> grid_;

    int toCharX(double x) const;
    int toCharY(double y) const;
    void setChar(int x, int y, char ch);
    void drawGridLines();
    void drawAxes();
    void drawConic(const Conic& conic);
    void drawPoints(const std::vector<Point>& points);
    void drawMouse(bool valid, double x, double y);
};

} // namespace ConsoleGeo