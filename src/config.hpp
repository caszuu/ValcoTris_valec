#pragma once

#include <Logic.hpp>
#include <array>

static constexpr int _cfg_width = 12; // screen width - make sure it is the same as the m_width in the library
static constexpr int _cfg_height = 10; // screen height - make sure it is the same as the m_height in the library
static constexpr bool _cfg_scroll_enable = true; // true | false
static constexpr bool _cfg_overflow_enable = (_cfg_scroll_enable || false); // true | false 
static constexpr bool _cfg_line_animation = true; // true | false

static constexpr int _cfg_joy_dead = 1024;
static constexpr int _cfg_joy_res = 4096;
static constexpr int _cfg_joy_cooldown = 150; // in ms

// color tables

static inline std::array<Rgb, 8> _cfg_tetris_colors {
    Rgb(0  , 0  , 0), // empty
    Rgb(255, 255, 0), // Square
    Rgb(255, 0  , 255), // I
    Rgb(0  , 0  , 255), // L
    Rgb(128, 255, 0), // L_Mirr
    Rgb(0  , 255, 0), // N
    Rgb(255, 0  , 0), // N_Mirr
    Rgb(0  , 255, 255) // T
};

/*static inline std::array<Rgb, 8> _cfg_tetris_colors {
    Rgb(0, 0, 0), // empty
    Rgb(255, 255, 0), // Square
    Rgb(0, 255, 255), // I
    Rgb(0, 0, 255), // L
    Rgb(255, 128, 0), // L_Mirr
    Rgb(255, 0, 0), // N
    Rgb(0, 255, 0), // N_Mirr
    Rgb(255, 0, 255) // T
};*/

static inline std::array<Rgb, 10> _cfg_number_colors {
    Rgb(0, 0, 0),
    Rgb(255, 0, 0),
    Rgb(255, 100, 0),
    Rgb(155, 255, 0),
    Rgb(0, 255, 0),
    Rgb(0, 255, 155),
    Rgb(0, 100, 255),
    Rgb(0, 0, 255),
    Rgb(100, 0, 255),
    Rgb(255, 0, 155)
};