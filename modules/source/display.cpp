// clang-format off
#include "stm32f756xx.h"
#include <vector>
#include "display.hpp"
// clang-format on

// Yeah... don't tell anyone.
void SmockerDelay(double s) {
    // PLL 336 MHz / 1 Mhz = 0.001 ms / 336 * 1000 = 336000
    s = (s * 336000);
    while (s) {
        s--;
    }
}

template <std::size_t N> Display<N>::Display() {
    using vect = std::vector<uint8_t>;
    constexpr uint16_t koled_command_pin_pa0{GPIO_MODER_MODER0_0};
    constexpr uint16_t koled_reset_pin_pd13{GPIO_MODER_MODER13_0};
    constexpr uint16_t koled_vci_en_pin_pd12{GPIO_MODER_MODER12_0};
    /* Enable GPIOD clock. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    // SET pins to drive mode
    GPIOD->MODER |= koled_reset_pin_pd13 | koled_vci_en_pin_pd12;

    /* Enable GPIOA clock. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    // SET pins to drive mode
    GPIOA->MODER |= koled_command_pin_pa0;
    GPIOA->OSPEEDR |= (0x03 << koled_command_pin_pa0);

    GPIOD->BSRR = GPIO_BSRR_BS_12;

    SmockerDelay(0.03);

    GPIOD->BSRR = GPIO_BSRR_BS_13;

    SmockerDelay(0.03);

    this->SendCommand(vect{0xFE, 0x07});
    this->SendCommand(vect{0x15, 0x04});
    this->SendCommand(vect{0xFE, 0x00});
    this->SendCommand(vect{0x35, 0x00});
    this->SendCommand(vect{0xC4, 0x80});
    // Config reduced color mode since MCU RAM is not enough to hold the frame buffer altogether.
    this->SendCommand(vect{0x3A, 0x72 /*0x72 0x75*/});
    this->SendCommand(vect{0x51, 0xFF});

    this->SendCommand(vect{0x2A, 0x00, 0x0E, 0x01, 0xD3});
    this->SendCommand(vect{0x2B, 0x00, 0x00, 0x01, 0xC5});
    this->SendCommand(vect{0x11});

    SmockerDelay(0.3);

    this->SendCommand(vect{0x29});
}

template <std::size_t N> void Display<N>::DmaFlush(const std::array<uint8_t, N>& data_array) {
    // LA-D2 // DATA/COMMAND modes.
    // LA-D3 // SCK
    // LA-D5 // MOSI
    // LA-D4 // Chip Select signal.
    this->SendCommand(0x2C);

    GPIOA->BSRR = GPIO_BSRR_BS_0; // Set COMMAND line to "data" mode.
    interface_.ClearCS();

    const uint16_t DMA_MAX{0xFFFF};
    uint32_t data = data_array.size();
    for (uint16_t i{}; data != 0; ++i) {
        if (data >= DMA_MAX) {
            data -= DMA_MAX;
            interface_.DmaFlush((data_array.data() + (i * DMA_MAX)), DMA_MAX);
        } else {
            interface_.DmaFlush((data_array.data() + (i * DMA_MAX)), data);
            data = 0;
        }
    }

    interface_.SetCS();
}

template <std::size_t N> void Display<N>::SendData(const uint8_t data) {
    GPIOA->BSRR = GPIO_BSRR_BS_0; // Set COMMAND line to "data" mode.
    interface_.ClearCS();
    interface_.Send(data);
    interface_.SetCS();
}

template <std::size_t N> void Display<N>::SendData(const std::vector<uint8_t>& data_array) {
    GPIOA->BSRR = GPIO_BSRR_BS_0; // Set COMMAND line to "data" mode.
    interface_.ClearCS();
    for (const auto& v : data_array) {
        interface_.Send(v);
    }
    interface_.SetCS();
}

template <std::size_t N> void Display<N>::SendData16(const std::vector<uint16_t>& data_array) {
    GPIOA->BSRR = GPIO_BSRR_BS_0; // Set COMMAND line to "data" mode.
    interface_.ClearCS();
    for (const auto& v : data_array) {
        interface_.Send(static_cast<uint8_t>((v >> 8) & 0xFF));
        interface_.Send(static_cast<uint8_t>(v & 0xFF));
    }
    interface_.SetCS();
}

template <std::size_t N> void Display<N>::SendFrame(const std::array<uint8_t, N>& data_array) {
    this->SendCommand(0x2C);

    GPIOA->BSRR = GPIO_BSRR_BS_0; // Set COMMAND line to "data" mode.
    interface_.ClearCS();
    for (const auto& v : data_array) {
        interface_.Send(v);
    }
    interface_.SetCS();
}

template <std::size_t N> void Display<N>::SendCommand(const uint8_t command) {
    GPIOA->BSRR = GPIO_BSRR_BR_0; // Set COMMAND line to "command" mode.
    interface_.ClearCS();
    interface_.Send(command);
    interface_.SetCS();
}

template <std::size_t N> void Display<N>::SendCommand(const std::vector<uint8_t>& instructions_array) {
    uint16_t c{0}; // 1 command N data
    interface_.ClearCS();
    for (const auto& v : instructions_array) {
        if (c > 0) {
            GPIOA->BSRR = GPIO_BSRR_BS_0; // Set COMMAND line to "data" mode.
        } else {
            GPIOA->BSRR = GPIO_BSRR_BR_0; // Set COMMAND line to "command" mode.
            c++;
        }

        interface_.Send(v);
    }

    interface_.SetCS();
}

template class Display<DISPLAY_RESOLUTION>;
