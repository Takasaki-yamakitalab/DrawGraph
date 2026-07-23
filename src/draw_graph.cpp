#include "DrawGraph/draw_graph.hpp"
#include <MathRenderer/bmp.hpp>
#include <cmath>

namespace draw_graph {
DrawGraph::DrawGraph(int width, int height)
    : width(width), height(height), draw_buffer(width * height, bmp::DEFAULT_BACKWARD_COLOR) {
}
DrawGraph::DrawGraph(int width, int height, int exit_code_show_graph, int exit_code_exit)
    : width(width), height(height), exit_code_show_graph(exit_code_show_graph), exit_code_exit(exit_code_exit), draw_buffer(width * height, bmp::DEFAULT_BACKWARD_COLOR) {
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
    while (cursor < 0) cursor += this->graphs.size() + 1;
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
    cursor = graphs.size() - 1;
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

void DrawGraph::plot(const std::vector<math_renderer::Token>& input, const std::string& var) {
    plot(input, var, 0);
}
void DrawGraph::plot(const std::vector<math_renderer::Token>& input, const std::string& var, uint32_t color) {
    if (input.size() == 0) return;

    int index = 0;
    std::string formula;
    math_renderer::make_math_str(input, index, formula);

    Graph graph;
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
    graph.formula = formula;
    graph.var = {var};
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
        int cur = 0;
        math_renderer::insert_input(input, cur, "f");
        math_renderer::insert_input(input, cur, "open_bracket");
        for (auto& str : graphs[cursor].var) {
            math_renderer::insert_input(input, cur, str);
        }
        math_renderer::insert_input(input, cur, "close_bracket");
        math_renderer::insert_input(input, cur, "equal");
        this->draw_math_input(3, 3, input);
    }

    if (var_name_temp.size() != 0) {
        this->draw_string(3, height - math_renderer::BASE_H - 3, var_name_temp);
    }

    calc.set_integral_N(1024);

    int x1, y1, x2, y2;
    for (int i = 0; i < this->graphs.size(); i++) {
        auto& graph = this->graphs[i];
        int lw = graph.lineWidth;
        if (i == this->cursor) lw *= 2;

        uint32_t color = graph.color;
        if (color == 0) {
            color = GRAPH_COLOR_PALETTE[i % 10];
        }

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
    if (this->cursor == this->graphs.size()) draw_axis();

    calc.set_integral_N();
    return 0;
}
}  // namespace draw_graph
