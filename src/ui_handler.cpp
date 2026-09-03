#include "ui_handler.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/canvas.hpp>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace ftxui;

namespace ConsoleGeo {

namespace {
    std::string fmtDouble(double val) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(4);
        oss << val;
        std::string s = oss.str();
        while (s.size() > 1 && s.back() == '0' && s[s.size() - 2] != '.') {
            s.pop_back();
        }
        if (s.size() > 1 && s.back() == '.') {
            s.pop_back();
        }
        return s;
    }

    double parseDouble(const std::string& s, double fallback) {
        try {
            std::size_t pos = 0;
            double val = std::stod(s, &pos);
            if (pos == 0) return fallback;
            return val;
        } catch (...) {
            return fallback;
        }
    }

    struct CanvasDims {
        int termCols;
        int termRows;
        int canvasPixelW;
        int canvasPixelH;
    };

    CanvasDims computeCanvasDims(const ScreenInteractive& screen) {
        int screenW = screen.dimx();
        int screenH = screen.dimy();

        if (screenW < 20) screenW = 80;
        if (screenH < 6)  screenH = 24;

        int termCols = static_cast<int>(screenW * 0.68);
        if (termCols < 10) termCols = 10;
        if (termCols > 400) termCols = 400;

        int termRows = screenH - 2;
        if (termRows < 3)  termRows = 3;
        if (termRows > 100) termRows = 100;

        int canvasPixelW = termCols * 2;
        int canvasPixelH = termRows * 4;

        return {termCols, termRows, canvasPixelW, canvasPixelH};
    }
}

UIHandler::UIHandler() {
    sH_      = fmtDouble(h_);
    sK_      = fmtDouble(k_);
    sR_      = fmtDouble(r_);
    sA_      = fmtDouble(a_);
    sB_      = fmtDouble(b_);
    sAng_    = fmtDouble(angle_);
    sP_      = fmtDouble(p_);
    sLX1_    = fmtDouble(lx1_);
    sLY1_    = fmtDouble(ly1_);
    sLX2_    = fmtDouble(lx2_);
    sLY2_    = fmtDouble(ly2_);
    sVXMin_  = fmtDouble(vxMin_);
    sVXMax_  = fmtDouble(vxMax_);
    sVYMin_  = fmtDouble(vyMin_);
    sVYMax_  = fmtDouble(vyMax_);
    status_  = "Ready. Press S=square, R=sqrt on focused input.";
    build();
    recalc();
}

void UIHandler::build() {
    using namespace ftxui;

    InputOption inputOpt;
    inputOpt.multiline = false;

    cH_     = Input(&sH_,     "h",     inputOpt);
    cK_     = Input(&sK_,     "k",     inputOpt);
    cR_     = Input(&sR_,     "r",     inputOpt);
    cA_     = Input(&sA_,     "a",     inputOpt);
    cB_     = Input(&sB_,     "b",     inputOpt);
    cAng_   = Input(&sAng_,   "angle", inputOpt);
    cP_     = Input(&sP_,     "p",     inputOpt);
    cLX1_   = Input(&sLX1_,   "x1",    inputOpt);
    cLY1_   = Input(&sLY1_,   "y1",    inputOpt);
    cLX2_   = Input(&sLX2_,   "x2",    inputOpt);
    cLY2_   = Input(&sLY2_,   "y2",    inputOpt);
    cVXMin_ = Input(&sVXMin_, "xMin",  inputOpt);
    cVXMax_ = Input(&sVXMax_, "xMax",  inputOpt);
    cVYMin_ = Input(&sVYMin_, "yMin",  inputOpt);
    cVYMax_ = Input(&sVYMax_, "yMax",  inputOpt);

    std::vector<std::string> typeEntries = {"Circle", "Ellipse", "Parabola", "Hyperbola"};
    cType_ = Toggle(&typeEntries, &typeIdx_);

    std::vector<std::string> dirEntries = {"Up", "Down", "Left", "Right"};
    cDir_ = Toggle(&dirEntries, &dirIdx_);

    cUpdate_ = Button("Update", [this] { recalc(); });

    auto inputPanel = Container::Vertical({
        cType_,
        cH_,   cK_,   cR_,
        cA_,   cB_,   cAng_,
        cP_,   cDir_,
        cLX1_, cLY1_, cLX2_, cLY2_,
        cVXMin_, cVXMax_, cVYMin_, cVYMax_,
        cUpdate_,
    });

    auto inputRenderer = Renderer(inputPanel, [&] {
        std::vector<Element> rows;

        rows.push_back(text("  Curve Type  ") | bold | center);
        rows.push_back(cType_->Render() | center);
        rows.push_back(separator());

        switch (typeIdx_) {
            case 0: {
                conicType_ = ConicType::Circle;
                rows.push_back(text("  Circle  ") | bold);
                rows.push_back(hbox({text(" h:"), cH_->Render()  | size(WIDTH, EQUAL, 8),
                                      text(" k:"), cK_->Render()  | size(WIDTH, EQUAL, 8),
                                      text(" r:"), cR_->Render()  | size(WIDTH, EQUAL, 8)}));
                break;
            }
            case 1: {
                conicType_ = ConicType::Ellipse;
                rows.push_back(text("  Ellipse  ") | bold);
                rows.push_back(hbox({text(" h:"), cH_->Render()    | size(WIDTH, EQUAL, 8),
                                      text(" k:"), cK_->Render()    | size(WIDTH, EQUAL, 8),
                                      text(" a:"), cA_->Render()    | size(WIDTH, EQUAL, 8)}));
                rows.push_back(hbox({text(" b:"), cB_->Render()    | size(WIDTH, EQUAL, 8),
                                      text(" ang:"), cAng_->Render() | size(WIDTH, EQUAL, 8)}));
                break;
            }
            case 2: {
                conicType_ = ConicType::Parabola;
                rows.push_back(text("  Parabola  ") | bold);
                rows.push_back(hbox({text(" h:"), cH_->Render()  | size(WIDTH, EQUAL, 8),
                                      text(" k:"), cK_->Render()  | size(WIDTH, EQUAL, 8),
                                      text(" p:"), cP_->Render()  | size(WIDTH, EQUAL, 8)}));
                rows.push_back(cDir_->Render() | center);
                switch (dirIdx_) {
                    case 0: pDir_ = ParabolaDirection::Up;    break;
                    case 1: pDir_ = ParabolaDirection::Down;  break;
                    case 2: pDir_ = ParabolaDirection::Left;  break;
                    case 3: pDir_ = ParabolaDirection::Right; break;
                }
                break;
            }
            case 3: {
                conicType_ = ConicType::Hyperbola;
                rows.push_back(text("  Hyperbola  ") | bold);
                rows.push_back(hbox({text(" h:"), cH_->Render()    | size(WIDTH, EQUAL, 8),
                                      text(" k:"), cK_->Render()    | size(WIDTH, EQUAL, 8),
                                      text(" a:"), cA_->Render()    | size(WIDTH, EQUAL, 8)}));
                rows.push_back(hbox({text(" b:"), cB_->Render()    | size(WIDTH, EQUAL, 8),
                                      text(" ang:"), cAng_->Render() | size(WIDTH, EQUAL, 8)}));
                break;
            }
        }

        rows.push_back(separator());
        rows.push_back(text("  Line  ") | bold);
        rows.push_back(hbox({text(" x1:"), cLX1_->Render() | size(WIDTH, EQUAL, 8),
                              text(" y1:"), cLY1_->Render() | size(WIDTH, EQUAL, 8)}));
        rows.push_back(hbox({text(" x2:"), cLX2_->Render() | size(WIDTH, EQUAL, 8),
                              text(" y2:"), cLY2_->Render() | size(WIDTH, EQUAL, 8)}));

        rows.push_back(separator());
        rows.push_back(text("  Viewport  ") | bold);
        rows.push_back(hbox({text(" xMin:"), cVXMin_->Render() | size(WIDTH, EQUAL, 8),
                              text(" xMax:"), cVXMax_->Render() | size(WIDTH, EQUAL, 8)}));
        rows.push_back(hbox({text(" yMin:"), cVYMin_->Render() | size(WIDTH, EQUAL, 8),
                              text(" yMax:"), cVYMax_->Render() | size(WIDTH, EQUAL, 8)}));

        rows.push_back(separator());
        rows.push_back(cUpdate_->Render() | center);
        rows.push_back(separator());
        rows.push_back(text("  Status  ") | bold);
        rows.push_back(text(status_));
        rows.push_back(text(""));
        rows.push_back(text("  S: square  R: sqrt  ") | dim);
        rows.push_back(text("  Mouse click on canvas  ") | dim);
        if (mouseValid_) {
            rows.push_back(text("  Mouse: (" + fmtDouble(mouseX_) + ", " + fmtDouble(mouseY_) + ")"));
        }

        return vbox(rows) | border | size(WIDTH, GREATER_THAN, 32);
    });

    auto canvasRenderer = Renderer([&] {
        auto dims = computeCanvasDims(screen_);
        return canvas(dims.canvasPixelW, dims.canvasPixelH,
                      [&](Canvas& c) {
            CoordinateCanvas coordCanvas(vxMin_, vxMax_, vyMin_, vyMax_,
                                         c.width(), c.height());
            coordCanvas.drawAllOn(c, makeConic(), pts_,
                                  mouseValid_, mouseX_, mouseY_);
        }) | flex | border;
    });

    auto mainContainer = Container::Horizontal({
        canvasRenderer,
        inputRenderer,
    });

    main_ = CatchEvent(mainContainer, [this](Event e) {
        return handleEvent(std::move(e));
    });
}

void UIHandler::recalc() {
    h_     = parseDouble(sH_,     h_);
    k_     = parseDouble(sK_,     k_);
    r_     = parseDouble(sR_,     r_);
    a_     = parseDouble(sA_,     a_);
    b_     = parseDouble(sB_,     b_);
    angle_ = parseDouble(sAng_,   angle_);
    p_     = parseDouble(sP_,     p_);
    lx1_   = parseDouble(sLX1_,   lx1_);
    ly1_   = parseDouble(sLY1_,   ly1_);
    lx2_   = parseDouble(sLX2_,   lx2_);
    ly2_   = parseDouble(sLY2_,   ly2_);
    vxMin_ = parseDouble(sVXMin_, vxMin_);
    vxMax_ = parseDouble(sVXMax_, vxMax_);
    vyMin_ = parseDouble(sVYMin_, vyMin_);
    vyMax_ = parseDouble(sVYMax_, vyMax_);

    if (vxMax_ <= vxMin_) vxMax_ = vxMin_ + 10.0;
    if (vyMax_ <= vyMin_) vyMax_ = vyMin_ + 10.0;

    if (r_  <= 0.0) r_  = 1.0;
    if (a_  <= 0.0) a_  = 1.0;
    if (b_  <= 0.0) b_  = 1.0;
    if (p_  <= 0.0) p_  = 1.0;

    Conic conic = makeConic();
    Line line   = makeLine();

    IntersectionSolver solver;
    IntersectionInfo info = solver.solve(conic, line);
    lastResult_ = info.result;
    pts_        = info.points;

    switch (info.result) {
        case IntersectionResult::Intersecting:
            status_ = "Intersecting: " + std::to_string(info.points.size()) + " point(s)";
            break;
        case IntersectionResult::Tangent:
            status_ = "Tangent: 1 point";
            break;
        case IntersectionResult::Separate:
            status_ = "Separate: no intersection";
            break;
    }
}

Conic UIHandler::makeConic() const {
    switch (conicType_) {
        case ConicType::Circle:
            return Conic::createCircle(h_, k_, r_);
        case ConicType::Ellipse:
            return Conic::createEllipse(h_, k_, a_, b_, angle_);
        case ConicType::Parabola:
            return Conic::createParabola(h_, k_, p_, pDir_);
        case ConicType::Hyperbola:
            return Conic::createHyperbola(h_, k_, a_, b_, angle_);
        default:
            return Conic::createCircle(h_, k_, r_);
    }
}

Line UIHandler::makeLine() const {
    return Line::fromTwoPoints(Point(lx1_, ly1_), Point(lx2_, ly2_));
}

bool UIHandler::handleEvent(ftxui::Event e) {
    if (e == Event::Character('s') || e == Event::Character('S')) {
        doSquare();
        recalc();
        return true;
    }

    if (e == Event::Character('r') || e == Event::Character('R')) {
        doSqrt();
        recalc();
        return true;
    }

    if (e == Event::Escape) {
        screen_.Exit();
        return true;
    }

    if (e.is_mouse()) {
        auto& m = e.mouse();
        if (m.button == Mouse::Left && m.motion == Mouse::Pressed) {
            auto dims = computeCanvasDims(screen_);

            if (m.x >= 0 && m.x < dims.termCols && m.y >= 0 && m.y < dims.termRows) {
                double ratioX = static_cast<double>(m.x) / (dims.termCols - 1);
                double ratioY = static_cast<double>(dims.termRows - 1 - m.y) / (dims.termRows - 1);
                mouseX_ = vxMin_ + ratioX * (vxMax_ - vxMin_);
                mouseY_ = vyMin_ + ratioY * (vyMax_ - vyMin_);
                mouseValid_ = true;
                return true;
            }
        }
    }

    return false;
}

void UIHandler::doSquare() {
    auto inputs = allInputs();
    for (auto& pair : inputs) {
        if ((*pair.first)->Focused()) {
            double val = parseDouble(*pair.second, 0.0);
            val = val * val;
            *pair.second = fmtDouble(val);
            status_ = "Square: " + fmtDouble(std::sqrt(val)) + " -> " + *pair.second;
            break;
        }
    }
}

void UIHandler::doSqrt() {
    auto inputs = allInputs();
    for (auto& pair : inputs) {
        if ((*pair.first)->Focused()) {
            double val = parseDouble(*pair.second, 0.0);
            if (val < 0.0) {
                status_ = "Sqrt: negative value, skipped";
                break;
            }
            val = std::sqrt(val);
            *pair.second = fmtDouble(val);
            status_ = "Sqrt: " + fmtDouble(val * val) + " -> " + *pair.second;
            break;
        }
    }
}

std::vector<std::pair<ftxui::Component*, std::string*>> UIHandler::allInputs() {
    return {
        {&cH_,     &sH_},
        {&cK_,     &sK_},
        {&cR_,     &sR_},
        {&cA_,     &sA_},
        {&cB_,     &sB_},
        {&cAng_,   &sAng_},
        {&cP_,     &sP_},
        {&cLX1_,   &sLX1_},
        {&cLY1_,   &sLY1_},
        {&cLX2_,   &sLX2_},
        {&cLY2_,   &sLY2_},
        {&cVXMin_, &sVXMin_},
        {&cVXMax_, &sVXMax_},
        {&cVYMin_, &sVYMin_},
        {&cVYMax_, &sVYMax_},
    };
}

void UIHandler::run() {
    try {
        screen_.Loop(main_);
    } catch (const std::exception& e) {
        std::cerr << "\n[EXCEPTION] " << e.what() << std::endl;
        auto dims = computeCanvasDims(screen_);
        std::cerr << "Screen dims: " << screen_.dimx() << " x " << screen_.dimy() << std::endl;
        std::cerr << "Canvas term: " << dims.termCols << " x " << dims.termRows << std::endl;
        std::cerr << "Canvas pixel: " << dims.canvasPixelW << " x " << dims.canvasPixelH << std::endl;
        std::cerr << "View range: [" << vxMin_ << ", " << vxMax_ << "] x ["
                  << vyMin_ << ", " << vyMax_ << "]" << std::endl;
        throw;
    }
}

} // namespace ConsoleGeo