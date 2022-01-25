// SPI_HPP
#pragma once

#include <cstdint>

/*
 * @brief Class to control and provide high-level interface for SPI Hardware.
 */
class Spi final {
    // Non movable nor copyable object.
    Spi(const Spi&) = delete;
    Spi& operator=(const Spi&) = delete;
    Spi(const Spi&&) = delete;
    Spi& operator=(const Spi&&) = delete;

  public:
    Spi();

    /*
     * @brief Function to flash the buffered data from the frame buffer to DMA.
     * It starts a chain of reaction where DMA takes the source address and flashes
     * the data that lie by the address to SPI hardware so the CPU remain free.
     *
     * @param frame_buffer A frame buffer pointer.
     * @param size An quantity of data to transfer.
     * @retval None
     */
    void DmaFlush(const uint8_t* frame_buffer, uint32_t size);

    /*
     * @brief A simple pooling way to send data through SPI Hardware used to configure
     * the display with the configuration commands.
     *
     * @param frame A byte to send.
     * @retval None
     */
    void Send(const uint8_t frame);

    /*
     * @brief A function to set CS pin level if hardware has one.
     *
     * @param None
     * @retval None
     */
    void SetCS();

    /*
     * @brief A function to clear CS pin level if hardware has one.
     *
     * @param None
     * @retval None
     */
    void ClearCS();
};

// SPI_HPP_END
