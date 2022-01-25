// DRAW_HPP
#pragma once

// clang-format off
#include <array>
#include <stdint.h>
#include <vector>
#include <cmath>
#include "config.hpp"
#include "display.hpp"
// clang-format on


enum struct Color : uint8_t {
    RED = 0xE0,
    GREEN = 0x1C,
    BLUE = 0x03,
    CYAN = BLUE + GREEN,
    YELLOW = RED + GREEN,
    PURPLE = RED + BLUE,
    VIOLET = BLUE | 0x80,
    WHITE = RED + BLUE + GREEN,
    default_color = WHITE
};


union Pixel {
    Pixel(Color color = Color::default_color) : RGB(color) {}

    struct Channel {
        uint8_t B : 2;
        uint8_t G : 3;
        uint8_t R : 3;
    } channel;

    Color RGB;
};


template <std::size_t N>
/// @brief Class provide graphical templates.
class Draw final {
    // Non movable nor copyable object.
    Draw(const Draw&) = delete;
    Draw& operator=(const Draw&) = delete;
    Draw(const Draw&&) = delete;
    Draw& operator=(const Draw&&) = delete;

    struct _Point2D {
        _Point2D(int16_t x = 0, int16_t y = 0, int16_t z = 0) : x(x), y(y), z(z) {}

        float x;
        float y;
        float z;
    };

    static constexpr float P_{3.14159};

    struct _ColoredFigure {
        _ColoredFigure(std::vector<_Point2D> points, Color color = Color::default_color)
            : points(points), color(color) {}
        std::vector<_Point2D> points{};
        Pixel color{};
    };

    Display<N> oled_;
    std::array<uint8_t, N>* const array_;
    std::vector<_ColoredFigure> obj_list_{};
    const uint16_t resolution_;
    uint16_t base_point_x_;
    uint16_t base_point_y_;
    Pixel color_depth(Pixel pix, uint16_t depth, uint16_t depth_range);

  public:
    Draw(std::array<uint8_t, N>* array, uint16_t x = 0, uint16_t y = 0)
        : array_(array), resolution_(static_cast<uint16_t>(std::sqrt(N))), base_point_x_(x), base_point_y_(y) {}

    int16_t Line2D(_Point2D p1, Color color = Color::default_color);
    int16_t Line2D(_Point2D p0, _Point2D p1, Color color = Color::default_color);
    int16_t Figure2D(std::vector<_Point2D> points, Color color = Color::default_color);
    int16_t FigureRotate(uint16_t figure_n, _Point2D rotation_center, float x_axis_angle = 0, float y_axis_angle = 0,
                         float z_axis_angle = 0);
    void Frame();
};

// DRAW_HPP_END
