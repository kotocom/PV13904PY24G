#include "draw.hpp"
#include "stm32f756xx.h"

template <std::size_t N> Pixel Draw<N>::color_depth(Pixel pix, uint16_t depth, uint16_t depth_range) {
    if (!depth) {
        return pix;
    }

    Pixel pix_base{Color::WHITE};

    uint8_t R_diff{static_cast<uint8_t>(pix_base.channel.R / static_cast<float>(depth_range) * depth)};
    uint8_t G_diff{static_cast<uint8_t>(pix_base.channel.G / static_cast<float>(depth_range) * depth)};
    uint8_t B_diff{static_cast<uint8_t>(pix_base.channel.B / static_cast<float>(depth_range) * depth)};

// Converting here to bit-field of reduced colors, and compiler doesn't like it.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    if (pix.channel.R <= R_diff) {
        pix.channel.R = 0;
    } else {
        pix.channel.R -= R_diff;
    }

    if (pix.channel.G <= G_diff) {
        pix.channel.G = 0;
    } else {
        pix.channel.G -= G_diff;
    }

    if (pix.channel.B <= B_diff) {
        pix.channel.B = 0;
    } else {
        pix.channel.B -= B_diff;
    }
#pragma GCC diagnostic push

    return pix;
}

template <std::size_t N> int16_t Draw<N>::Line2D(_Point2D p1, Color color) {
    if (p1.x > resolution_ || p1.y > resolution_) {
        return -1;
    }

    this->obj_list_.push_back(_ColoredFigure({{_Point2D(), p1}, color}));

    return 0;
}

template <std::size_t N> int16_t Draw<N>::Line2D(_Point2D p0, _Point2D p1, Color color) {
    if (p0.x > resolution_ || p0.y > resolution_ || p1.x > resolution_ || p1.y > resolution_) {
        return -1;
    }

    this->obj_list_.push_back(_ColoredFigure({{p0, p1}, color}));

    return 0;
}

template <std::size_t N> int16_t Draw<N>::Figure2D(std::vector<_Point2D> points, Color color) {
    for (auto& v : points) {
        if (v.x > resolution_ || v.y > resolution_) {
            return -1;
        }
    }

    this->obj_list_.push_back(_ColoredFigure(points, color));
    this->obj_list_.back().points.push_back(this->obj_list_.back().points[0]); // Terminate the figure shape.

    return 0;
}

template <std::size_t N>
int16_t Draw<N>::FigureRotate(uint16_t figure_n, _Point2D rotation_center, float x_axis_angle, float y_axis_angle,
                              float z_axis_angle) {
    // We don't check if the resulted point will be out of screen.
    if ((obj_list_.size() < static_cast<uint16_t>(figure_n + 1)) ||
        (rotation_center.x > resolution_ || rotation_center.y > resolution_ || rotation_center.z > resolution_)) {
        return -1;
    }

    // Using rotation matrices to rotate the points over a rotation_center point.
    for (auto& v : obj_list_[figure_n].points) {
        // "Zeroing" the point to the rotation center.
        float x_temp = v.x = v.x - rotation_center.x;
        float y_temp = v.y = v.y - rotation_center.y;
        float z_temp = v.z = v.z - rotation_center.z;
        float x_temp1;
        float y_temp1;
        float z_temp1;

        if (x_axis_angle) {
            // Convert the angle to radians.
            float x_rads = x_axis_angle * P_ / 180;
            //
            //      |1,      0,       0 | |X|
            // Rx = |0, cos(j), -sin(j) | |Y|
            //      |0, sin(j), cos(j)  | |Z|
            //
            y_temp1 = (y_temp * cosf(x_rads)) - (z_temp * sinf(x_rads));
            z_temp1 = (y_temp * sinf(x_rads)) + (z_temp * cosf(x_rads));
            y_temp = y_temp1;
            z_temp = z_temp1;
        }

        if (y_axis_angle) {
            // Convert the angle to radians.
            float y_rads = y_axis_angle * P_ / 180;
            //
            //      | cos(j), 0, sin(j) | |X|
            // Ry = |      0, 1,      0 | |Y|
            //      |-sin(j), 0, cos(j) | |Z|
            //
            x_temp1 = static_cast<float>((x_temp * cosf(y_rads)) + (z_temp * sin(y_rads)));
            z_temp1 = (z_temp * cosf(y_rads)) - (x_temp * sinf(y_rads));
            x_temp = x_temp1;
            z_temp = z_temp1;
        }

        if (z_axis_angle) {
            // Convert the angle to radians.
            float z_rads = z_axis_angle * P_ / 180;
            //
            //      | cos(j), -sin(j), 0 | |X|
            // Rz = | sin(j),  cos(j), 0 | |Y|
            //      |      0,       0, 1 | |Z|
            //
            x_temp1 = (x_temp * cosf(z_rads)) - (y_temp * sinf(z_rads));
            y_temp1 = static_cast<float>((x_temp * sin(z_rads)) + (y_temp * cos(z_rads)));
            x_temp = x_temp1;
            y_temp = y_temp1;
        }

        v.x = x_temp + rotation_center.x;
        v.y = y_temp + rotation_center.y;
        v.z = z_temp + rotation_center.z;
    }

    return 0;
}

template <std::size_t N> void Draw<N>::Frame() {
    if (array_) {
        LED_TOGGLE;
        std::fill_n(reinterpret_cast<uint64_t*>(array_->data()), N / 8, 0);
        LED_TOGGLE;
        LED_TOGGLE;

        for (auto& v : this->obj_list_) {
            for (std::size_t i{1}; i < v.points.size(); ++i) {
                uint16_t it{0};
                uint16_t base_x;
                uint16_t base_y;
                uint16_t base_z;
                uint16_t delta_x = static_cast<uint16_t>(abs(static_cast<int16_t>(v.points[i - 1].x - v.points[i].x)));
                uint16_t delta_y = static_cast<uint16_t>(abs(static_cast<int16_t>(v.points[i - 1].y - v.points[i].y)));

                if (delta_x > delta_y) {
                    float y_scalar;
                    float z_scalar;
                    if (v.points[i - 1].x < v.points[i].x) {
                        base_x = static_cast<uint16_t>(v.points[i - 1].x);
                        base_y = static_cast<uint16_t>(v.points[i - 1].y);
                        base_z = static_cast<uint16_t>(v.points[i - 1].z);
                        y_scalar = static_cast<float>(v.points[i].y - v.points[i - 1].y) / delta_x;
                        z_scalar = static_cast<float>(v.points[i].z - v.points[i - 1].z) / delta_x;
                    } else {
                        base_x = static_cast<uint16_t>(v.points[i].x);
                        base_y = static_cast<uint16_t>(v.points[i].y);
                        base_z = static_cast<uint16_t>(v.points[i].z);
                        y_scalar = static_cast<float>(v.points[i - 1].y - v.points[i].y) / delta_x;
                        z_scalar = static_cast<float>(v.points[i - 1].z - v.points[i].z) / delta_x;
                    }

                    while (it <= delta_x) {
                        (*array_)[(static_cast<uint16_t>(it * y_scalar + base_y) * resolution_) + base_x + it] =
                            static_cast<uint8_t>(
                                color_depth(v.color, static_cast<uint16_t>(it * z_scalar + base_z), 150).RGB);
                        it++;
                    }
                } else {
                    float x_scalar;
                    float z_scalar;
                    if (v.points[i - 1].y < v.points[i].y) {
                        base_x = static_cast<uint16_t>(v.points[i - 1].x);
                        base_y = static_cast<uint16_t>(v.points[i - 1].y);
                        base_z = static_cast<uint16_t>(v.points[i - 1].z);
                        x_scalar = static_cast<float>(v.points[i].x - v.points[i - 1].x) / delta_y;
                        z_scalar = static_cast<float>(v.points[i].z - v.points[i - 1].z) / delta_y;
                    } else {
                        base_x = static_cast<uint16_t>(v.points[i].x);
                        base_y = static_cast<uint16_t>(v.points[i].y);
                        base_z = static_cast<uint16_t>(v.points[i].z);
                        x_scalar = static_cast<float>(v.points[i - 1].x - v.points[i].x) / delta_y;
                        z_scalar = static_cast<float>(v.points[i - 1].z - v.points[i].z) / delta_y;
                    }

                    while (it <= delta_y) {
                        (*array_)[(static_cast<uint16_t>(it + base_y) * resolution_) +
                                  static_cast<uint16_t>(it * x_scalar + base_x)] =
                            static_cast<uint8_t>(
                                color_depth(v.color, static_cast<uint16_t>(it * z_scalar + base_z), 150).RGB);
                        it++;
                    }
                }
            }
        }
    }

    LED_TOGGLE;
    oled_.DmaFlush(*array_);
}

template class Draw<DISPLAY_RESOLUTION>;
