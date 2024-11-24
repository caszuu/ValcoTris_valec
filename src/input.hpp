#pragma once

#include <cstdint>
#include "utils.hpp"

enum class dpad_direction : uint8_t {
    up = 0,
    right,
    down,
    left,
};

struct joystick_event {
    Vec3 current_input; // processed input, should be used for game input
    Vec3 raw_input; // raw input received from joystick

    // emulated dpad, bitfield for if dirs are out of their deadzone (bit 0 to 3, top, right, bottom, left)
    uint8_t dpad_state;

    inline bool is_dpad_active(dpad_direction dir) const {
        return (dpad_state >> static_cast<uint8_t>(dir)) & 1;
    }
};

using input_callback_t = void (*)(const joystick_event& event);