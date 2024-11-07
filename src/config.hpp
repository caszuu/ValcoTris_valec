#pragma once

static constexpr int _cfg_width = 12; // screen width - make sure it is the same as the m_width in the library
static constexpr int _cfg_height = 10; // screen height - make sure it is the same as the m_height in the library
static constexpr bool _cfg_scroll_enable = true; // true | false
static constexpr bool _cfg_overflow_enable = (_cfg_scroll_enable || false); // true | false 
static constexpr bool _cfg_line_animation = true; // true | false

static constexpr int _cfg_joy_dead = 1024;
static constexpr int _cfg_joy_res = 4096;
static constexpr int _cfg_joy_cooldown = 150; // in ms