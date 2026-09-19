#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "conic.hpp"
#include "intersection.hpp"
#include "canvas_draw.hpp"
#include <string>
#include <vector>

namespace ConsoleGeo {

class UIHandler {
public:
    UIHandler();
    void run();

private:
    ConicType conicType_ = ConicType::Circle;
    double h_ = 0.0, k_ = 0.0, r_ = 2.0;
    double a_ = 3.0, b_ = 2.0, angle_ = 0.0;
    double p_ = 1.0;
    ParabolaDirection pDir_ = ParabolaDirection::Up;
    double lx1_ = -5.0, ly1_ = -5.0, lx2_ = 5.0, ly2_ = 5.0;
    double vxMin_ = -10.0, vxMax_ = 10.0, vyMin_ = -10.0, vyMax_ = 10.0;

    std::vector<Point> pts_;
    IntersectionResult lastResult_ = IntersectionResult::Separate;

    double mouseX_ = 0.0, mouseY_ = 0.0;
    bool mouseValid_ = false;

    std::string status_;

    std::string sH_, sK_, sR_, sA_, sB_, sAng_, sP_;
    std::string sLX1_, sLY1_, sLX2_, sLY2_;
    std::string sVXMin_, sVXMax_, sVYMin_, sVYMax_;

    ftxui::Component cH_, cK_, cR_, cA_, cB_, cAng_, cP_;
    ftxui::Component cLX1_, cLY1_, cLX2_, cLY2_;
    ftxui::Component cVXMin_, cVXMax_, cVYMin_, cVYMax_;
    ftxui::Component cType_, cDir_, cUpdate_;
    ftxui::Component main_;

    ftxui::ScreenInteractive screen_ = ftxui::ScreenInteractive::TerminalOutput();

    int typeIdx_ = 0;
    int dirIdx_ = 0;
    std::vector<std::string> typeEntries_;
    std::vector<std::string> dirEntries_;

    void build();
    void recalc();
    Conic makeConic() const;
    Line makeLine() const;
    bool handleEvent(ftxui::Event e);
    void doSquare();
    void doSqrt();
    std::vector<std::pair<ftxui::Component*, std::string*>> allInputs();
};

} // namespace ConsoleGeo