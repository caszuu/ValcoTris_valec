#pragma once

#include "colors.hpp"
#include "global_vars.hpp"
#include "piece.hpp"
#include "config.hpp"
#include "utils.hpp"
#include <array>
#include <math.h>

extern std::array<Rgb, 8> colors;

class Map {
public:
    Map() {}
    ~Map() {} // default constructors, creates empty map

    void draw() {
        for (int x = 0; x < _cfg_width; x++) {
            for (int y = 0; y < _cfg_height; y++) {
                if (placedPixels[x][y] != 0) {
                    int p_x = x + view_offset;
                    if (_cfg_overflow_enable == true || _cfg_scroll_enable == true) {
                        while (p_x < 0) {
                            p_x += _cfg_width;
                        }
                        while (p_x >= _cfg_width) {
                            p_x -= _cfg_width;
                        }
                    } else {
                        if (p_x < 0) {
                            p_x = 0;
                        }
                        if (p_x >= _cfg_width) {
                            p_x = _cfg_width - 1;
                        }
                    }
                    display.setColor(p_x, y, colors[placedPixels[x][y]]);
                }
            }
        }
    }

    void operator<<(const Piece& piece) { // merge piece
        std::vector<PiecePixel> pixels {};
        piece.getShape(pixels);

        for (auto& p : pixels) {
            placedPixels[p.pos.x][p.pos.y] = p.value;
        }
    }

    bool checkCollision(const Piece& piece) const {
        std::vector<PiecePixel> pixels {};
        piece.getShape(pixels);

        for (auto& p : pixels) {
            if (placedPixels[p.pos.x][p.pos.y] != 0 && p.pos.y >= 2) {
                return true;
            }
        }

        return false;
    }

    void checkLines() {
        for (int i = 0; i < _cfg_height; i++) {
            bool full = true;

            for (int x = 0; x < _cfg_width; x++) {
                if (placedPixels[x][i] == 0) {
                    full = false;
                }
            }

            if (full) {
                full_lines += 1;
                if (_cfg_line_animation) {

                    lineAnimation(i);
                }
                moveLines(i);
            }
        }
    }
    void lineAnimation(int row) {
        for (int o = 0; o < ((_cfg_width / 2) - 1); o++) {
            int time = 50 - (o * 2);
            for (int i = o; i >= -1; i--) {
                std::cout << i << "\n";
                std::cout << (_cfg_width - i) - 1 << "\n";

                display.setColor(i, row, display.at(i + 1, row));
                display.setColor(i + 1, row, Rgb(0, 0, 0));

                display.setColor((_cfg_width - i) - 1, row, display.at((_cfg_height - i) - 2, row));
                display.setColor((_cfg_width - i) - 2, row, Rgb(0, 0, 0));
                display.show(brightness);
                delay(time);
            }
        }
    }

    void moveLines(int i) {
        for (int y = i; y > 0; y--) {
            for (int x = 0; x < _cfg_width; x++) {
                placedPixels[x][y] = placedPixels[x][y - 1];
            }
        }
        //printf("line\n");
    }

    void clearMap() {
        for (int y = 0; y < _cfg_height; y++) {
            for (int x = 0; x < _cfg_width; x++) {
                placedPixels[x][y] = 0;
            }
        }
    }

    char placedPixels[_cfg_width][_cfg_height];
    int full_lines = 0;

private:
};