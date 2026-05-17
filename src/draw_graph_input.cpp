#include "DrawGraph/draw_graph.hpp"

namespace draw_graph {
int DrawGraph::process_input(std::string str) {
    static int edit_varname = 0;
    if (!edit_varname) {
        if (str == "plus") {
            this->zoom(1.1);
        } else if (str == "minus") {
            this->zoom(0.9);
        } else if (str == "ac") {
            this->clear_all();
            return exit_code_exit;
        } else if (str == "del") {
            this->delete_graph();
        } else if (str == "left") {
            this->move_cursor(-1);
        } else if (str == "right") {
            this->move_cursor(1);
        } else if (str == "0") {
            return exit_code_exit;
        } else if (str == "7") {
            this->shift(0, 10);
        } else if (str == "3") {
            this->shift(0, -10);
        } else if (str == "1") {
            this->shift(-10, 0);
        } else if (str == "9") {
            this->shift(10, 0);
        } else if (str == "5") {
            this->reset_scale();
        } else if (str == "4") {
            this->shift(-7, 7);
        } else if (str == "8") {
            this->shift(7, 7);
        } else if (str == "6") {
            this->shift(7, -7);
        } else if (str == "2") {
            this->shift(-7, -7);
        } else if (str == "plot") {
            this->apply_var_name();
            edit_varname = 0;
        } else {
            edit_varname = 1;
        }
    }
    if (edit_varname) {
        if (str == "plot") {
            this->apply_var_name();
            edit_varname = 0;
        } else if (str == "ac") {
            this->delete_var_name();
        } else if (str == "del") {
            if (!this->var_name_temp.empty()) this->var_name_temp.pop_back();
        }
        else {
            this->insert_var_name(str);
        }
    }
    return exit_code_show_graph;
}
}  // namespace draw_graph
