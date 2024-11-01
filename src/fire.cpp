#include <Logic.hpp>
#include <iostream>
#include <array>

#include "config.hpp"

extern int brightness;

const int MAX_TEMPERATURE = 33;

bool fire_active = true;

std::array< Rgb, MAX_TEMPERATURE + 1 > temperatures{
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

std::array< std::array< int, _cfg_width>, _cfg_height+1 > firePixels;

void seedFire() {
    for (int x = 0; x != _cfg_width; x++)
        firePixels[x][_cfg_height] = MAX_TEMPERATURE;
}

void stopFire() {
    for (int x = 0; x != _cfg_width; x++)
        firePixels[x][_cfg_height] = 0;
}

void spreadFire(int x, int y) {
    int newTemperature = firePixels[x][y] - random(1, 8);
    if (newTemperature < 0)
        newTemperature = 0;
    int newX = x + random(-2, 1);
    if (newX < 0 || newX >= _cfg_width)
        return;
    firePixels[newX][y - 1] = newTemperature;
}

void doFire() {
    for(int x = 0; x < _cfg_width; x++) {
        for (int y = 0; y < _cfg_height; y++) {
            spreadFire(x, y+1);
        }
    }
}

void showFire() {
    for(int x = 0; x < _cfg_width; x++) {
        for (int y = 0; y < _cfg_height; y++) {
            Rgb c = temperatures[firePixels[x][y]];
            c.linearize();
            display.setColor(x, y, c);
        }
    }
}


void stop_fire(){
    stopFire();
    delay(2000);
    fire_active = false;
}

int fire_main() {
    fire_active = true;
    seedFire();

    while( fire_active ) {
           
        doFire();
        showFire();
        display.show(brightness);

        delay(50);
    }

    return 1;
}