#include "spi.hpp"
#include "stm32f756xx.h"

// The concrete class handle SPI1 interface
Spi::Spi() {                                                  // LA-D2 // DATA/COMMAND modes.
    constexpr uint16_t kspi1_sck_pa5{GPIO_MODER_MODER5_Pos};  // D13 // LA-D3
    constexpr uint16_t kspi1_miso_pa6{GPIO_MODER_MODER6_Pos}; // D12
    constexpr uint16_t kspi1_mosi_pa7{GPIO_MODER_MODER7_Pos}; // D11 // LA-D5
    constexpr uint16_t kspi1_cs_pa4{GPIO_MODER_MODER4_Pos};   // D10 // LA-D4
    constexpr uint16_t kalternate_pin_mode{2};
    constexpr uint16_t kalternate_function_spi1{5};
    constexpr uint16_t kL{0};
    constexpr uint16_t kHISPEED{0x03};

    /* Enable GPIOA clock. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    // SET pins to alternate mode and CS as push/pull.
    GPIOA->MODER |= (kalternate_pin_mode << kspi1_sck_pa5) | (kalternate_pin_mode << kspi1_mosi_pa7) |
                    (kalternate_pin_mode << kspi1_miso_pa6) | (1 << kspi1_cs_pa4);
    GPIOA->AFR[kL] = (kalternate_function_spi1 << (kspi1_sck_pa5 * kalternate_pin_mode)) |
                     (kalternate_function_spi1 << (kspi1_miso_pa6 * kalternate_pin_mode)) |
                     (kalternate_function_spi1 << (kspi1_mosi_pa7 * kalternate_pin_mode));
    GPIOA->OSPEEDR |= (kHISPEED << kspi1_sck_pa5) | (kHISPEED << kspi1_mosi_pa7) | (kHISPEED << kspi1_cs_pa4);

    /* SPI DMA Configuration block. */
    // SPI1 TX: DMA2:: Stream3, Stream5
    // SPI1 RX: DMA2:: Stream2, Stream0

    /* Enable clock for the DMA2. */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    DMA2_Stream5->CR &= ~DMA_SxCR_EN;
    while ((DMA2_Stream5->CR & DMA_SxCR_EN) != 0) {
    } // Disable DMA before configuring.

    /* Clear interrupt flag if exists. */
    DMA2->HIFCR = DMA_HIFCR_CTCIF5;

    /* Setting destination address register. */
    DMA2_Stream5->PAR = (volatile uint32_t)(&SPI1->DR);

    /* Configure DMA channel, mem to peripheral, memory increment and circular buffer mode. */
    DMA2_Stream5->CR |= (DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0) | DMA_SxCR_MINC |
                        /*DMA_SxCR_PFCTRL*/ /*DMA_SxCR_CIRC |*/ DMA_SxCR_DIR_0 | DMA_SxCR_TCIE;

    /* Enable SPI1 clock. */
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    /* Config SPI1 as Master, speed/64 */
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI |
                 /* SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0 | */ SPI_CR1_MSTR;
    /* Config SPI1 data frame 8 bit */
    SPI1->CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;

    /* Enable DMA mode for SPI1 */
    SPI1->CR2 |= SPI_CR2_TXDMAEN; // | SPI_CR2_TXEIE;

    /* Switch SPI1 ON */
    SPI1->CR1 |= SPI_CR1_SPE;

    this->SetCS();
}

void Spi::DmaFlush(const uint8_t* frame_buffer, uint32_t size) {
    /* Start the DMA transmission. */

    /* Setting source address register. */
    DMA2_Stream5->M0AR = (volatile uint32_t)frame_buffer;

    /* Clear the interrupt flag if exists. */
    DMA2_Stream5->NDTR = size;

    ///* Set the amount of bytes to transfer. */
    DMA2->HIFCR = DMA_HIFCR_CTCIF5;

    DMA2_Stream5->CR |= DMA_SxCR_EN;

    while ((DMA2->HISR & DMA_HISR_TCIF5) == 0) {
    } // Wait until Transfer ends.
    DMA2->HIFCR = DMA_HIFCR_CTCIF5;

    DMA2_Stream5->CR &= ~DMA_SxCR_EN;
}

void Spi::Send(const uint8_t frame) {
    while (!(SPI1->SR & SPI_SR_TXE)) {
    } // Wait until Transfer buffer Empty
    // Yeah, DR register is sensitive to how many bytes compiler pushes to a bus.
    // Even tho the SPI is configured to 8 bit frame, if compiler pushes 0x000F,
    // then... we have a problem and 16 clock ticks.
    *((volatile uint8_t*)&SPI1->DR) = frame; // write data
    while (SPI1->SR & SPI_SR_BSY) {
    } // Wait for transmission done
}

void Spi::SetCS() {
    GPIOA->BSRR = GPIO_BSRR_BS_4; // Takes Chip/slave select line High
}

void Spi::ClearCS() {
    GPIOA->BSRR = GPIO_BSRR_BR_4; // Takes Chip/slave select line Low
}
