#pragma once

#include "input.hpp"

namespace tetris {
    extern int view_offset;

    void tetris_input_cb(const joystick_event& ev);
    int tetris_main();
}