#include <iostream>
#include <string>
#include "../include/DrawGraph/draw_graph.hpp"

int main(void) {
    sci_calc::SciCalc calc;
    draw_graph::DrawGraph plot(400, 300);

    plot.plot("@lnx", "x");
    plot.refresh(calc);
    bmp::save32bitBMP("result0.bmp", 400, 300, plot.get_draw_buffer());

    plot.shift(300, 0);
    plot.refresh(calc);
    bmp::save32bitBMP("result1.bmp", 400, 300, plot.get_draw_buffer());

    plot.zoom(2);
    std::cout << "tics: " << plot.get_tics() << std::endl;
    plot.refresh(calc);
    bmp::save32bitBMP("result2.bmp", 400, 300, plot.get_draw_buffer());

    bmp::save32bitBMP("result.bmp", 400, 300, plot.get_draw_buffer());

    std::cout << "Press Enter to exit" << std::endl;
    std::cin.get();
    return 0;
}
