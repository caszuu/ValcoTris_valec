#include <Logic.hpp>
#include <iostream>
#include <array>

#include "config.hpp"

static bool fire_active = true;
static std::array< std::array< int, _cfg_width>, _cfg_height+1 > firePixels;

static void seedFire() {
    for (int x = 0; x != _cfg_width; x++)
        firePixels[x][_cfg_height] = MAX_TEMPERATURE;
}

static void stopFire() {
    for (int x = 0; x != _cfg_width; x++)
        firePixels[x][_cfg_height] = 0;
}

static void spreadFire(int x, int y) {
    int newTemperature = firePixels[x][y] - random(1, 8);
    if (newTemperature < 0)
        newTemperature = 0;
    int newX = x + random(-2, 1);
    if (newX < 0 || newX >= _cfg_width)
        return;
    firePixels[newX][y - 1] = newTemperature;
}

static void doFire() {
    for(int x = 0; x < _cfg_width; x++) {
        for (int y = 0; y < _cfg_height; y++) {
            spreadFire(x, y+1);
        }
    }
}

static void showFire() {
    for(int x = 0; x < _cfg_width; x++) {
        for (int y = 0; y < _cfg_height; y++) {
            Rgb c = temperatures[firePixels[x][y]];
            c.linearize();
            display.setColor(x, y, c);
        }
    }
}

// fire app api

void fire_input_cb(const joystick_event& ev) {
    if (!ev.dpad_state) return; // if no input, do not stop 

    stopFire();
    delay(2000);
    fire_active = false;
}

void fire_main() {
    fire_active = true;
    seedFire();

    while( fire_active ) {
           
        doFire();
        showFire();
        display.show(brightness);

        delay(50);
    }
}