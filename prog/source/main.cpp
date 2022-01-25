#include "stm32f756xx.h"
#include <cstdint>

#include "draw.hpp"

int main() {
    std::array<uint8_t, DISPLAY_RESOLUTION> array{};
    Draw<DISPLAY_RESOLUTION> draw(&array);

    // Create the figure by coordinate points.
    // It's just a line-based-shape. Not a complete figure. Good enough for the display test.
    draw.Figure2D({{154,154},{154,300},{300, 300},{300,154},{154, 154},
            {154,154,-154},{154,300,-154},{300,300,-154},{300,154,-154}, {154,154,-154}}, Color::RED);

    // Enable LED activity pin.
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER |= GPIO_MODER_MODER0_0;

    float angle{0.1f};

    while(true) {

        draw.FigureRotate(0, { 227, 227} , -angle * 2.0f, angle + 2.1f, angle + 1.3f);

        LED_TOGGLE;
        draw.Frame();
    }
}
