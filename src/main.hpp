#pragma once

#include <cstdint>

#include "config.hpp"
#include "input.hpp"
#include "utils.hpp"

// Valec MAC    = C8:2B:96:B5:CA:84
// Joystick MAC = C8:2B:96:B8:C9:C0

/* Main device state, top-level info and firmware entry is here */

class device_state {
public:
    device_state() : active_game{0}, next_valid_input_time{0} { }
    ~device_state() = default;

    void enter_launcher();
    void process_input(const void* data);

private:
    uint8_t active_game;
    uint32_t next_valid_input_time;

    input_callback_t active_callback = nullptr;
};