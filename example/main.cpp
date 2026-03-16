#include <iostream>
#include <string>
#include "../include/DrawGraph/draw_graph.hpp"

int main(void) {
    bmp::Image img = bmp::loadBMP1bit("dot_font_x11y23.bmp");
    std::vector<bmp::Glyph> glyphs;
    for (int i = 0; i < math_renderer::LETTER_NUM; i++) {
        glyphs.push_back(bmp::extractGlyph(img, i, math_renderer::BASE_W, math_renderer::BASE_H));
    }

    draw_graph::DrawGraph plot(400, 300);
    plot.load_glyph_list(glyphs);

    plot.plot("@lnx", "x");
    plot.refresh();
    bmp::save32bitBMP("result0.bmp", 400, 300, plot.get_draw_buffer());

    plot.shift(300, 0);
    plot.refresh();
    bmp::save32bitBMP("result1.bmp", 400, 300, plot.get_draw_buffer());

    plot.zoom(2);
    std::cout << "tics: " << plot.get_tics() << std::endl;
    plot.refresh();
    bmp::save32bitBMP("result2.bmp", 400, 300, plot.get_draw_buffer());

    std::cout << "Press Enter to exit" << std::endl;
    bmp::save32bitBMP("result.bmp", 400, 300, plot.get_draw_buffer());
    std::cin.get();
    return 0;
}
