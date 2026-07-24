#include "DrawGraph/draw_graph.hpp"

namespace draw_graph {

int DrawGraph::draw_number(int posX, int posY, double num, uint32_t color) {
    char buff[256];
#ifdef ANDROID
    sprintf(buff, "%g", num);
#else
    sprintf_s(buff, sizeof(buff), "%g", num);
#endif
    int cnt = 0;
    for (int i = 0; i < strlen(buff); i++) {
        switch (buff[i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                bmp::drawGlyph(this->draw_buffer.data(), this->width, this->height, posX, posY,
                               this->glyphs[buff[i] - '0'], 1, color);
                posX += math_renderer::BASE_W;
                cnt++;
                break;
            case '.':
                bmp::drawGlyph(this->draw_buffer.data(), this->width, this->height, posX, posY,
                               this->glyphs[dot_font::LETTER_PERIOD], 1, color);
                posX += math_renderer::BASE_W;
                cnt++;
                break;
            case '-':
                bmp::drawGlyph(this->draw_buffer.data(), this->width, this->height, posX, posY,
                               this->glyphs[dot_font::LETTER_MINUS], 1, color);
                posX += math_renderer::BASE_W;
                cnt++;
                break;
            case 'e':
                bmp::drawGlyph(this->draw_buffer.data(), this->width, this->height, posX, posY,
                               this->glyphs[dot_font::LETTER_e], 1, color);
                posX += math_renderer::BASE_W;
                cnt++;
            case 'E':
                bmp::drawGlyph(this->draw_buffer.data(), this->width, this->height, posX, posY,
                               this->glyphs[dot_font::LETTER_E], 1, color);
                posX += math_renderer::BASE_W;
                cnt++;
            default:
                break;
        }
    }
    return cnt;
}

int DrawGraph::get_number_width(double num) {
    char buff[256];
#ifdef ANDROID
    sprintf(buff, "%g", num);
#else
    sprintf_s(buff, sizeof(buff), "%g", num);
#endif
    return strlen(buff) * math_renderer::BASE_W;
}

int DrawGraph::draw_char(int posX, int posY, char c, uint32_t color) {
    if ('0' <= c && c <= '9') {
        draw_letter(posX, posY, c - '0' + dot_font::LETTER_0, color);
        return 1;
    }
    if ('A' <= c && c <= 'Z') {
        draw_letter(posX, posY, c - 'A' + dot_font::LETTER_A, color);
        return 1;
    }
    if ('a' <= c && c <= 'z') {
        draw_letter(posX, posY, c - 'a' + dot_font::LETTER_a, color);
        return 1;
    }
    return 0;
}
int DrawGraph::draw_char(int posX, int posY, const std::string& c, uint32_t color) {
    if (c.size() == 1) return draw_char(posX, posY, c[0], color);
    for (int i = 0; i < LETTER_LIST.size(); i++) {
        if (c == LETTER_LIST[i]) {
            draw_letter(posX, posY, i, color);
            return 1;
        }
    }
    return 0;
}

int DrawGraph::draw_string(int posX, int posY, const std::string& str, uint32_t color) {
    int cnt = 0;
    for (auto& c : str) {
        int num = draw_char(posX, posY, c, color);
        cnt += num;
        posX += math_renderer::BASE_W * num;
    }
    return cnt;
}
int DrawGraph::draw_string(int posX, int posY, const std::vector<std::string>& str, uint32_t color) {
    int cnt = 0;
    for (auto& c : str) {
        int num = draw_char(posX, posY, c, color);
        cnt += num;
        posX += math_renderer::BASE_W * num;
    }
    return cnt;
}

void DrawGraph::draw_letter(int posX, int posY, int letter_id, uint32_t color) {
    bmp::drawGlyph(this->draw_buffer.data(), this->width, this->height, posX, posY, this->glyphs[letter_id], 1, color);
}

void DrawGraph::draw_math_input(int posX, int posY, std::vector<math_renderer::Token>& input, uint32_t color) {
    math_renderer::MathRenderer mr(this->width, this->height);
    mr.load_glyph_list(this->glyphs);
    math_renderer::LayoutEngine le(input);
    math_renderer::LayoutBox box = le.layout(math_renderer::SMALL_SCALE);
    box.shift(posX, posY);
    int endX = std::max(this->width, posX + box.width);
    int endY = std::max(this->height, posY + box.ascent + box.descent);
    mr.draw_tokens(box.tokens);
    mr.draw_special_objects(box.spobj);
    uint32_t* data = mr.get_draw_buffer();
    for (int y = posY; y < endY; y++) {
        for (int x = posX; x < endX; x++) {
            if (data[y * this->width + x] != 0) draw_buffer[y * this->width + x] = data[y * this->width + x];
        }
    }
}
}  // namespace draw_graph
