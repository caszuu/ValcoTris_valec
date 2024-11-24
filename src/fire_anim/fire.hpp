#pragma once

#include "input.hpp"

namespace fire_anim {
    static constexpr int MAX_TEMPERATURE = 33;

    static inline std::array< Rgb, MAX_TEMPERATURE + 1 > temperatures{
        Rgb(0,  0,   0),
        Rgb(7,  10,  10),
        Rgb(7,  31,  7),
        Rgb(15, 47,  7),
        Rgb(15, 71,  7),
        Rgb(23, 87,  7),
        Rgb(31, 103, 7),
        Rgb(31, 119, 7),
        Rgb(39, 143, 7),
        Rgb(47, 159, 7),
        Rgb(63, 175, 7),
        Rgb(71, 191, 7),
        Rgb(71, 199, 7),
        Rgb(79, 223, 7),
        Rgb(87, 223, 7),
        Rgb(87, 223, 7),
        Rgb(95, 215, 7),
        Rgb(95, 215, 7),
        Rgb(103,215,  15),
        Rgb(111,207,  15),
        Rgb(119,207,  15),
        Rgb(120,207,  15),
        Rgb(120,207,  23),
        Rgb(120,199,  23),
        Rgb(125,199,  23),
        Rgb(125,199,  31),
        Rgb(130,191,  31),
        Rgb(130,191,  31),
        Rgb(130,191,  39),
        Rgb(135,191,  47),
        Rgb(135,183,  47),
        Rgb(140,183,  47),
        Rgb(140,183,  55),
        Rgb(255,255,  255)
    };

    void fire_input_cb(const joystick_event& ev);
    int fire_main();
}