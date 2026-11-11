#include "DrawGraph/draw_graph.hpp"
#include <BitmapTool/bmp.hpp>
#include <DotFont/dot_font_x11y23.bmp.hpp>
#include <MathRenderer/bmp.hpp>
#include <SciCalc/sci_calc.hpp>
#include <SciCalc/sci_calc_define.hpp>
#include <cmath>
#include <cstdint>
#include "DrawGraph/draw_graph_defines.hpp"

namespace draw_graph {
DrawGraph::DrawGraph(int width, int height)
    : width(width), height(height), draw_buffer(width * height, bmp::DEFAULT_BACKWARD_COLOR) {
    load_glyphs();
}
DrawGraph::DrawGraph(int width, int height, int exit_code_show_graph, int exit_code_exit)
    : width(width), height(height), exit_code_show_graph(exit_code_show_graph), exit_code_exit(exit_code_exit), draw_buffer(width * height, bmp::DEFAULT_BACKWARD_COLOR) {
    load_glyphs();
}

void DrawGraph::load_glyphs() {
    bmp::Image img = bmp::loadBMP1bit((char*)dot_font::img, sizeof(dot_font::img));
    this->glyphs.clear();
    for (int i = 0; i < dot_font::LETTER_NUM; i++) {
         glyphs.push_back(bmp::extractGlyph(img, i, dot_font::BASE_W, dot_font::BASE_H));
    }
}
void DrawGraph::load_glyph_list(const std::vector<bmp::Glyph>& glyph_list) {
    glyphs = glyph_list;
}
uint32_t* DrawGraph::get_draw_buffer() {
    return this->draw_buffer.data();
}

void DrawGraph::zoom(double mag) {
    int old_scale = this->scale;
    this->scale *= mag;
    if (mag > 1.0 && old_scale == this->scale) this->scale++;
    if (mag < 1.0 && old_scale == this->scale) this->scale--;
    if (this->scale < SCALE_MIN) this->scale = SCALE_MIN;
    if (this->scale > SCALE_MAX) this->scale = SCALE_MAX;
    this->offsetX *= (double)this->scale / (double)old_scale;
    this->offsetY *= (double)this->scale / (double)old_scale;
}
void DrawGraph::shift(int dx, int dy) {
    this->offsetX += dx;
    this->offsetY += dy;
}
void DrawGraph::reset_scale() {
    this->scale = SCALE_DEFAULT;
    this->offsetX = 0;
    this->offsetY = 0;
}
void DrawGraph::clear_all() {
    reset_scale();
    graphs.clear();
    var_name_temp.clear();
    cursor = 0;
    for (auto& buf : this->draw_buffer) {
        buf = bmp::DEFAULT_BACKWARD_COLOR;
    }
}

int DrawGraph::move_cursor(int move) {
    if (this->graphs.size() == 0) {
        this->cursor = 0;
        return 0;
    }

    cursor += move;
    while (cursor < 0) cursor += (int)this->graphs.size() + 1;
    cursor = cursor % (this->graphs.size() + 1);
    return cursor;
}
int DrawGraph::delete_graph() {
    if (this->graphs.size() == 0) {
        this->cursor = 0;
        return 0;
    }
    if (this->graphs.size() <= this->cursor) return 0;
    this->graphs.erase(this->graphs.begin() + cursor);
    if (graphs.size() == 0) {
        cursor = 0;
        return 0;
    }
    cursor = (int)graphs.size() - 1;
    return cursor;
}
std::vector<math_renderer::Token> DrawGraph::get_graph_input() {
    if (this->graphs.size() == 0) return std::vector<math_renderer::Token>();
    if (this->graphs.size() <= this->cursor) return std::vector<math_renderer::Token>();
    return this->graphs[this->cursor].input;
}

void DrawGraph::draw_axis() {
    math_renderer::LayoutBox box;
    math_renderer::SpecialObject obj;

    const int margin = 2;

    // position of the origin in display
    int originX = this->posX2displayX(0.0);
    int originY = this->posY2displayY(0.0);
    int axisX = std::max(0, std::min(originX, this->width - 1));
    int axisY = std::max(0, std::min(originY, this->height - 1));

    double tics = this->get_tics();
    int step = tics * this->scale;

    double posX = std::floor(displayX2posX(0) / tics) * tics;
    int displayX = posX2displayX(posX);
    while (displayX < this->width) {
        if (std::abs(displayX - originX) > step / 2) {
            bmp::drawLine(draw_buffer.data(), width, height, displayX, axisY - 3, displayX, axisY + 3, GRID_MAJOR_COLOR);
            if (originY + margin < 0) {
                draw_number(displayX + margin, margin, std::round(displayX2posX(displayX) / tics) * tics, TEXT_COLOR);
            } else if (this->height < originY + margin + math_renderer::BASE_H + margin) {
                draw_number(displayX + margin, this->height - margin - math_renderer::BASE_H, std::round(displayX2posX(displayX) / tics) * tics, TEXT_COLOR);
            } else {
                draw_number(displayX + margin, originY + margin, std::round(displayX2posX(displayX) / tics) * tics, TEXT_COLOR);
            }
        }
        for (int i = 1; i < 10; i++) {
            bmp::drawLine(draw_buffer.data(), width, height, displayX + i * step / 10, axisY - 2, displayX + i * step / 10, axisY + 2, GRID_MINOR_COLOR);
        }
        displayX += step;
    }

    double posY = std::ceil(displayY2posY(0) / tics) * tics;
    int displayY = posY2displayY(posY);
    while (displayY < this->height) {
        if (std::abs(displayY - originY) > step / 2) {
            bmp::drawLine(draw_buffer.data(), width, height, axisX - 3, displayY, axisX + 3, displayY, GRID_MAJOR_COLOR);
            int num_width = get_number_width(std::round(displayY2posY(displayY) / tics) * tics);
            if (originX + margin < 0) {
                draw_number(margin, displayY + margin, std::round(displayY2posY(displayY) / tics) * tics, TEXT_COLOR);
            } else if (this->width < originX + margin + num_width + margin) {
                draw_number(this->width - margin - num_width, displayY + margin, std::round(displayY2posY(displayY) / tics) * tics, TEXT_COLOR);
            } else {
                draw_number(originX + margin, displayY + margin, std::round(displayY2posY(displayY) / tics) * tics, TEXT_COLOR);
            }
        }
        for (int i = 1; i < 10; i++) {
            bmp::drawLine(draw_buffer.data(), width, height, axisX - 2, displayY + i * step / 10, axisX + 2, displayY + i * step / 10, GRID_MINOR_COLOR);
        }
        displayY += step;
    }

    // origin
    draw_number(originX + margin, originY + margin, 0, TEXT_COLOR);

    bmp::drawLine(draw_buffer.data(), width, height, 0, axisY, this->width - 1, axisY, AXIS_COLOR);
    bmp::drawLine(draw_buffer.data(), width, height, axisX, 0, axisX, this->height - 1, AXIS_COLOR);
}

void DrawGraph::draw_graph(Graph& graph, bool bold, uint32_t color, sci_calc::SciCalc& calc) {
    int x1, x2, y1, y2;
    int lw = graph.lineWidth;
    if (bold) lw *= 2;

    std::string var;
    for (auto& str : graph.var) {
        var += str;
    }
    sci_calc::Complex memory = calc.get_memory(var);
    for (x1 = 0; x1 < this->width; x1++) {
        sci_calc::Complex ans = 0.0;
        if (!graph.points.count(displayX2posX(x1))) {
            calc.assign_memory(var, displayX2posX(x1));
            ans = calc.calc(graph.formula);
            graph.points[displayX2posX(x1)] = ans;
        }
        else {
            ans = graph.points[displayX2posX(x1)];
        }
        if (std::isnan(ans.real()) || std::abs(ans.imag()) > 1e-15) continue;
        y1 = posY2displayY(ans.real());
        break;
    }
    while (x1 < this->width) {
        x2 = x1 + 1;
        sci_calc::Complex ans = 0.0;
        if (!graph.points.count(displayX2posX(x2))) {
            calc.assign_memory(var, displayX2posX(x2));
            ans = calc.calc(graph.formula);
            graph.points[displayX2posX(x2)] = ans;
        }
        else {
            ans = graph.points[displayX2posX(x2)];
        }
        y2 = posY2displayY(ans.real());
        if (y2 != GRAPH_NAN && std::abs(ans.imag()) < 1e-15) {
            if (y1 == GRAPH_NAN) y1 = y2;
            bmp::drawLine(this->draw_buffer.data(), width, height, x1, y1, x2, y2, lw, color);
            y1 = y2;
        }
        x1 = x2;
    }
    calc.assign_memory(var, memory);
}
static int marchingSquare(double v0, double v1, double v2, double v3) {
    int ret = 0;
    if (v0 <= 0) ret += 0b1000;
    if (v1 <= 0) ret += 0b0100;
    if (v2 <= 0) ret += 0b0010;
    if (v3 <= 0) ret += 0b0001;
    return ret;
}
static inline sci_calc::Complex internalDiv(sci_calc::Complex p1, sci_calc::Complex p2, double w1, double w2) {
    return (p1 * fabs(w2) + p2 * fabs(w1)) / (fabs(w1) + fabs(w2));
}
void DrawGraph::draw_impgraph(Graph &graph, bool bold, uint32_t color, sci_calc::SciCalc &calc, int step) {
    sci_calc::Complex mem1 = calc.get_memory("x");
    sci_calc::Complex mem2 = calc.get_memory("y");

    int lw = graph.lineWidth;
    if (bold) lw *= 2;
    for (int y = 0; y < this->height + step; y += step) {
        for (int x = 0; x < this->width + step; x += step) {
            sci_calc::Complex pos(displayX2posX(x), displayY2posY(y));
            if (graph.points.count(pos) == 0) {
                calc.assign_memory("x", pos.real());
                calc.assign_memory("y", pos.imag());
                graph.points[pos] = calc.calc(graph.formula);
            }
        }
    }
    for (int i = 0; i < this->height; i += step) {
        for (int j = 0; j < this->width; j += step) {
            sci_calc::Complex p0(displayX2posX(j), displayY2posY(i));
            sci_calc::Complex p1(displayX2posX(j), displayY2posY(i + step));
            sci_calc::Complex p2(displayX2posX(j + step), displayY2posY(i + step));
            sci_calc::Complex p3(displayX2posX(j + step), displayY2posY(i));
            double v0 = graph.points[p0].real();
            double v1 = graph.points[p1].real();
            double v2 = graph.points[p2].real();
            double v3 = graph.points[p3].real();
            sci_calc::Complex pos1, pos2;
            switch (marchingSquare(v0, v1, v2, v3)) {
                case 0b0001: case 0b1110:
                    pos1 = internalDiv(p3, p0, v3, v0);
                    pos2 = internalDiv(p3, p2, v3, v2);
                    bmp::drawLine(this->draw_buffer.data(), width, height, posX2displayX(pos1.real()), posY2displayY(pos1.imag()), posX2displayX(pos2.real()), posY2displayY(pos2.imag()), lw, color);
                    break;
                case 0b0010: case 0b1101:
                    pos1 = internalDiv(p2, p3, v2, v3);
                    pos2 = internalDiv(p2, p1, v2, v1);
                    bmp::drawLine(this->draw_buffer.data(), width, height, posX2displayX(pos1.real()), posY2displayY(pos1.imag()), posX2displayX(pos2.real()), posY2displayY(pos2.imag()), lw, color);
                    break;
                case 0b0100: case 0b1011:
                    pos1 = internalDiv(p1, p2, v1, v2);
                    pos2 = internalDiv(p1, p0, v1, v0);
                    bmp::drawLine(this->draw_buffer.data(), width, height, posX2displayX(pos1.real()), posY2displayY(pos1.imag()), posX2displayX(pos2.real()), posY2displayY(pos2.imag()), lw, color);
                    break;
                case 0b1000: case 0b0111:
                    pos1 = internalDiv(p0, p1, v0, v1);
                    pos2 = internalDiv(p0, p3, v0, v3);
                    bmp::drawLine(this->draw_buffer.data(), width, height, posX2displayX(pos1.real()), posY2displayY(pos1.imag()), posX2displayX(pos2.real()), posY2displayY(pos2.imag()), lw, color);
                    break;
                case 0b0110: case 0b1001:
                    pos1 = internalDiv(p0, p1, v0, v1);
                    pos2 = internalDiv(p2, p3, v2, v3);
                    bmp::drawLine(this->draw_buffer.data(), width, height, posX2displayX(pos1.real()), posY2displayY(pos1.imag()), posX2displayX(pos2.real()), posY2displayY(pos2.imag()), lw, color);
                    break;
                case 0b0011: case 0b1100:
                    pos1 = internalDiv(p0, p3, v0, v3);
                    pos2 = internalDiv(p1, p2, v1, v2);
                    bmp::drawLine(this->draw_buffer.data(), width, height, posX2displayX(pos1.real()), posY2displayY(pos1.imag()), posX2displayX(pos2.real()), posY2displayY(pos2.imag()), lw, color);
                    break;
                case 0b0101:
                    pos1 = internalDiv(p0, p1, v0, v1);
                    pos2 = internalDiv(p0, p3, v0, v3);
                    bmp::drawLine(this->draw_buffer.data(), width, height, posX2displayX(pos1.real()), posY2displayY(pos1.imag()), posX2displayX(pos2.real()), posY2displayY(pos2.imag()), lw, color);
                    pos1 = internalDiv(p2, p3, v2, v3);
                    pos2 = internalDiv(p2, p1, v2, v1);
                    bmp::drawLine(this->draw_buffer.data(), width, height, posX2displayX(pos1.real()), posY2displayY(pos1.imag()), posX2displayX(pos2.real()), posY2displayY(pos2.imag()), lw, color);
                case 0b1010:
                    pos1 = internalDiv(p1, p2, v1, v2);
                    pos2 = internalDiv(p1, p0, v1, v0);
                    bmp::drawLine(this->draw_buffer.data(), width, height, posX2displayX(pos1.real()), posY2displayY(pos1.imag()), posX2displayX(pos2.real()), posY2displayY(pos2.imag()), lw, color);
                    pos1 = internalDiv(p3, p0, v3, v0);
                    pos2 = internalDiv(p3, p2, v3, v2);
                    bmp::drawLine(this->draw_buffer.data(), width, height, posX2displayX(pos1.real()), posY2displayY(pos1.imag()), posX2displayX(pos2.real()), posY2displayY(pos2.imag()), lw, color);
                case 0b0000: case 0b1111: default:
                    break;
            }
        }
    }
    calc.assign_memory("x", mem1);
    calc.assign_memory("y", mem2);
}

void DrawGraph::plot(const std::vector<math_renderer::Token>& input, const std::string& var) {
    plot(input, var, 0);
}
void DrawGraph::plot(const std::vector<math_renderer::Token>& input, const std::string& var, uint32_t color) {
    if (input.size() == 0) return;

    int index = 0;
    std::string formula;
    math_renderer::make_math_str(input, index, formula);

    Graph graph;
    graph.type = EXPLICIT_2D;
    graph.input = input;
    graph.formula = formula;
    graph.var = {var};
    graph.color = color;
    graph.lineWidth = this->lineWidth;
    this->graphs.push_back(graph);
    this->cursor = this->graphs.size() - 1;
}
void DrawGraph::plot(const std::string& formula, const std::string& var) {
    plot(formula, var, 0);
}
void DrawGraph::plot(const std::string& formula, const std::string& var, uint32_t color) {
    if (formula.size() == 0) return;
    Graph graph;
    graph.type = EXPLICIT_2D;
    graph.formula = formula;
    graph.var = {var};
    graph.color = color;
    graph.lineWidth = this->lineWidth;
    this->graphs.push_back(graph);
    this->cursor = this->graphs.size() - 1;
}
void DrawGraph::iplot(const std::vector<math_renderer::Token>& input) {
    iplot(input, 0);
}
void DrawGraph::iplot(const std::vector<math_renderer::Token>& input, uint32_t color) {
    if (input.size() == 0) return;

    int index = 0;
    std::string formula;
    math_renderer::make_math_str(input, index, formula);

    Graph graph;
    graph.type = IMPLICIT_2D;
    graph.input = input;
    graph.formula = formula;
    graph.color = color;
    graph.lineWidth = this->lineWidth;
    this->graphs.push_back(graph);
    this->cursor = this->graphs.size() - 1;
}
void DrawGraph::iplot(const std::string& formula) {
    iplot(formula, 0);
}
void DrawGraph::iplot(const std::string& formula, uint32_t color) {
    if (formula.size() == 0) return;

    Graph graph;
    graph.type = IMPLICIT_2D;
    graph.formula = formula;
    graph.color = color;
    graph.lineWidth = this->lineWidth;
    this->graphs.push_back(graph);
    this->cursor = this->graphs.size() - 1;
}

void DrawGraph::insert_var_name(const std::string& str) {
    for (auto& letter : LETTER_LIST) {
        if (str == letter) {
            var_name_temp.push_back(str);
            return;
        }
    }
}
void DrawGraph::delete_var_name() {
    var_name_temp.clear();
}
void DrawGraph::apply_var_name() {
    if (graphs.size() == 0) return;
    if (cursor < 0 || graphs.size() <= cursor) return;
    if (var_name_temp.size() == 0) var_name_temp = {"x"};
    graphs[cursor].var = var_name_temp;
    graphs[cursor].points.clear();
    var_name_temp.clear();
}

int DrawGraph::refresh(sci_calc::SciCalc& calc) {
    for (auto& buf : this->draw_buffer) {
        buf = bmp::DEFAULT_BACKWARD_COLOR;
    }

    if (this->cursor != this->graphs.size()) draw_axis();
    if (graphs.size() == 0) return -1;

    if (0 <= cursor && cursor < graphs.size()) {
        auto input = graphs[cursor].input;
        if (graphs[cursor].type == EXPLICIT_2D) {
            int cur = 0;
            math_renderer::insert_input(input, cur, "f");
            math_renderer::insert_input(input, cur, "open_bracket");
            for (auto& str : graphs[cursor].var) {
                math_renderer::insert_input(input, cur, str);
            }
            math_renderer::insert_input(input, cur, "close_bracket");
            math_renderer::insert_input(input, cur, "equal");
        }
        if (graphs[cursor].type == IMPLICIT_2D) {
            int cur = input.size();
            math_renderer::insert_input(input, cur, "equal");
            math_renderer::insert_input(input, cur, "0");
        }
        this->draw_math_input(3, 3, input);
    }

    if (var_name_temp.size() != 0) {
        this->draw_string(3, height - math_renderer::BASE_H - 3, var_name_temp);
    }

    calc.set_integral_N(1024);

    for (int i = 0; i < this->graphs.size(); i++) {
        auto& graph = this->graphs[i];
        uint32_t color = graph.color;
        if (color == 0) color = GRAPH_COLOR_PALETTE[i % 10];
        if (graph.type == EXPLICIT_2D) draw_graph(graph, i == this->cursor, color, calc);
        if (graph.type == IMPLICIT_2D) draw_impgraph(graph, i == this->cursor, color, calc);
    }
    if (this->cursor == this->graphs.size()) draw_axis();

    calc.set_integral_N();
    return 0;
}
}  // namespace draw_graph
