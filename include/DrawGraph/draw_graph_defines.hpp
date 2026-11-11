#ifndef DRAW_GRAPH_DEFINES_HPP
#define DRAW_GRAPH_DEFINES_HPP

#include <cstdint>
#include <vector>
#include <string>

namespace draw_graph {
constexpr int SCALE_MAX = 1000;
constexpr int SCALE_MIN = 5;
constexpr int SCALE_DEFAULT = 50;
constexpr int GRAPH_DIVISION_NUMBER_X = 6;

constexpr uint32_t AXIS_COLOR = 0xFFAAAAAA;
constexpr uint32_t GRID_MAJOR_COLOR = 0xFFAAAAAA;
constexpr uint32_t GRID_MINOR_COLOR = 0xFF5A5A5A;
constexpr uint32_t TEXT_COLOR = 0xFFDDDDDD;

constexpr int GRAPH_INFINITY = 10000;
constexpr int GRAPH_NAN = GRAPH_INFINITY + 1;

const std::vector<std::string> LETTER_LIST{
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "_", "^", "+", "-", "times", "div", "=", "%", ";", ":",
    "<", ">", "<=", ">=", ":=", "=:", "deg", "'", "\"", "|", "||", "infty",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
    "!", ",", ".", "?", "(", ")", "{", "}", "[", "]", "/", "\\",
    "Alpha", "Beta", "Gamma", "Delta", "Epsilon", "Zeta", "Eta", "Theta", "Iota", "Kappa", "Lambda", "Mu",
    "Nu", "Xi", "Omicron", "Pi", "Rho", "Sigma", "Tau", "Upsilon", "Phi", "Chi", "Psi", "Omega",
    "alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta", "iota", "kappa", "lambda", "mu",
    "nu", "xi", "omicron", "pi", "rho", "sigma", "tau", "upsilon", "phi", "chi", "psi", "omega",
    "sqrt", "(", ")", "integral",
    " "};

#ifdef __ANDROID__
constexpr uint32_t GRAPH_COLOR_PALETTE[] = {
    0xFFA8784C,
    0xFFB2B772,
    0xFF4BA254,
    0xFF6BCFB5,
    0xFF2CA8EC,
    0xFF1885F5,
    0xFF5657E4,
    0xFFA69DFF,
    0xFFA279B2,
    0xFFD66A9D};
#else
constexpr uint32_t GRAPH_COLOR_PALETTE[] = {
    0xFF4C78A8,
    0xFF72B7B2,
    0xFF54A24B,
    0xFFB5CF6B,
    0xFFECA82C,
    0xFFF58518,
    0xFFE45756,
    0xFFFF9DA6,
    0xFFB279A2,
    0xFF9D6AD6};
#endif

enum GRAPH_TYPE {
    EXPLICIT_2D,
    IMPLICIT_2D,
};
}  // namespace draw_graph

#endif
