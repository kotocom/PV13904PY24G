// DISPLAY_HPP
#pragma once

#include "config.hpp"
#include "spi.hpp"
#include <array>
#include <cstdint>

template <std::size_t N>
/// @brief Class to control and provide high-level interface for oled screen functional.
class Display final {
    // Non movable nor copyable object.
    Display(const Display&) = delete;
    Display& operator=(const Display&) = delete;
    Display(const Display&&) = delete;
    Display& operator=(const Display&&) = delete;

    Spi interface_;

  public:
    Display();

    void DmaFlush(const std::array<uint8_t, N>& data_array);
    void SendData(const uint8_t data);
    void SendData16(const std::vector<uint16_t>& data_array);
    void SendData(const std::vector<uint8_t>& data_array);
    void SendFrame(const std::array<uint8_t, N>& data_array);
    void SendCommand(const uint8_t command);
    void SendCommand(const std::vector<uint8_t>& instructions_array);
};

// DISPLAY_HPP_END
