#ifndef DRAW_GRAPH_HPP
#define DRAW_GRAPH_HPP

#include <SciCalc/sci_calc.hpp>
#include <MathRenderer/math_renderer.hpp>
#include <unordered_map>
#include "draw_graph_defines.hpp"

namespace draw_graph {
struct Graph {
    std::vector<math_renderer::Token> input;
    std::string formula;
    std::vector<std::string> var;
    uint32_t color;
    uint32_t lineWidth;
    std::unordered_map<double, sci_calc::Complex> points;
};

class DrawGraph {
   private:
    int scale = SCALE_DEFAULT;  // number of pixels per division
    int offsetX = 0;
    int offsetY = 0;
    int lineWidth = 3;
    int width;
    int height;
    int cursor = 0;
    int exit_code_show_graph = 2;
    int exit_code_exit = -1;
    std::vector<uint32_t> draw_buffer;
    std::vector<bmp::Glyph> glyphs;
    std::vector<Graph> graphs;
    std::vector<std::string> var_name_temp;

   public:
    DrawGraph(int width, int height);
    DrawGraph(int width, int height, int exit_code_show_graph, int exit_code_exit);
    void load_glyphs();
    void load_glyph_list(const std::vector<bmp::Glyph>& glyph_list);
    uint32_t* get_draw_buffer();

    void zoom(double mag);
    void shift(int dx, int dy);
    void reset_scale();
    void clear_all();

    int move_cursor(int move);
    int delete_graph();

    int posX2displayX(double x);
    int posY2displayY(double x);
    double displayX2posX(int x);
    double displayY2posY(int x);
    double get_tics();
    std::vector<math_renderer::Token> get_graph_input();

    int draw_number(int posX, int posY, double num, uint32_t color = TEXT_COLOR);
    int get_number_width(double num);
    int draw_char(int posX, int posY, char c, uint32_t color = TEXT_COLOR);
    int draw_char(int posX, int posY, const std::string& c, uint32_t color = TEXT_COLOR);
    int draw_string(int posX, int posY, const std::string& str, uint32_t color = TEXT_COLOR);
    int draw_string(int posX, int posY, const std::vector<std::string>& str, uint32_t color = TEXT_COLOR);
    void draw_letter(int posX, int posY, int letter_id, uint32_t color = TEXT_COLOR);
    void draw_math_input(int posX, int posY, std::vector<math_renderer::Token>& input, uint32_t color = TEXT_COLOR);

    void draw_axis();
    void plot(const std::vector<math_renderer::Token>& input, const std::string& var);
    void plot(const std::vector<math_renderer::Token>& input, const std::string& var, uint32_t color);
    void plot(const std::string& formula, const std::string& var);
    void plot(const std::string& formula, const std::string& var, uint32_t color);
    void insert_var_name(const std::string& str);
    void delete_var_name();
    void apply_var_name();
    int refresh(sci_calc::SciCalc& calc);

    int process_input(std::string str);
    int process_touch(int x, int y);
    int process_swipe(int dx, int dy);
};
}  // namespace draw_graph

#endif
