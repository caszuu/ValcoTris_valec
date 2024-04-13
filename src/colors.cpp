#include "colors.hpp"
#include <Logic.hpp>

std::array<Rgb, 8> colors {
    Rgb(0, 0, 0), // empty
    Rgb(255, 255, 0), // Square
    Rgb(0, 255, 255), // I
    Rgb(0, 0, 255), // L
    Rgb(255, 128, 0), // L_Mirr
    Rgb(255, 0, 0), // N
    Rgb(0, 255, 0), // N_Mirr
    Rgb(255, 0, 255) // T
};

std::array<Rgb, 10> number_colors {
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