#include "DrawGraph/draw_graph.hpp"
#include <cmath>

namespace draw_graph {

int DrawGraph::posX2displayX(double x) {
    if (std::isnan(x)) return GRAPH_NAN;
    double displayX = this->width / 2.0 + x * this->scale - this->offsetX;
    if (displayX < -GRAPH_INFINITY) return -GRAPH_INFINITY;
    if (displayX > GRAPH_INFINITY) return GRAPH_INFINITY;
    return std::round(displayX);
}
int DrawGraph::posY2displayY(double y) {
    if (std::isnan(y)) return GRAPH_NAN;
    double displayY = this->height / 2.0 - y * this->scale + this->offsetY;
    if (displayY < -GRAPH_INFINITY) return -GRAPH_INFINITY;
    if (displayY > GRAPH_INFINITY) return GRAPH_INFINITY;
    return std::round(displayY);
}

double DrawGraph::displayX2posX(int x) {
    return (double)(x - this->width / 2.0 + this->offsetX) / (double)this->scale;
}
double DrawGraph::displayY2posY(int y) {
    return (double)(-y + this->height / 2.0 + this->offsetY) / (double)this->scale;
}

double DrawGraph::get_tics() {
    double tics = (double)this->width / (double)this->scale / (double)GRAPH_DIVISION_NUMBER_X;
    if (tics <= 0.1) return 0.1;
    if (tics <= 0.2) return 0.2;
    if (tics <= 0.5) return 0.5;
    if (tics <= 1.0) return 1.0;
    if (tics <= 2.0) return 2.0;
    if (tics <= 5.0) return 5.0;
    if (tics <= 10.0) return 10.0;
    if (tics <= 20.0) return 20.0;
    if (tics <= 50.0) return 50.0;
    return 100.0;
}

}  // namespace draw_graph
