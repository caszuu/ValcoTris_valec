#include "piece.hpp"
#include "config.hpp"

#include "tetris.hpp"

namespace tetris {
    Piece::Piece(PieceKind type, Vec2 pos, const uint8_t& rot)
        : // initializator list, calls the construtor for objects in the class
        position { pos }
        , rotation { rot }
        , type { type } {
        confirm();
    }

    Piece::~Piece() {}

    void Piece::lower() {
        position.y++;
        lowerLast = true;
    }
    void Piece::rotateClockwise() {
        if (rotation + 1 != 4) {
            rotation++;
        } else {
            rotation = 0;
        }
    }
    void Piece::rotateCounterClockwise() {
        if (rotation != 0) {
            rotation--;
        } else {
            rotation = 3;
        }
    }

    void Piece::confirm() {
        lastPosition = position;
        lastRotation = rotation;
    }

    void Piece::undo() {
        position = lastPosition;
        rotation = lastRotation;
    }

    void Piece::draw() {
        std::vector<PiecePixel> pixels;
        renderShape(pixels);

        for (auto& p : pixels) {
            if (p.value != 0) {
                int p_x = p.pos.x + view_offset;
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
                display.setColor(p_x, p.pos.y, _cfg_tetris_colors[p.value]);
            }
        }
    }

    void Piece::renderShape(std::vector<PiecePixel>& pixels) const {
        // load pixels

        for (int32_t x = 0; x < Pieces[type][0].size(); x++) {
            for (int32_t y = 0; y < Pieces[type].size(); y++) {

                if (Pieces[type][y][x] == 1) {
                    pixels.push_back({ 1, Vec2 { x - PiecesCenters[type].x, y - PiecesCenters[type].y } }); // constructs the pixel inside the vector
                } else if (Pieces[type][y][x] == 2) {
                    pixels.push_back({ 2, Vec2 { x - PiecesCenters[type].x, y - PiecesCenters[type].y } });
                } else if (Pieces[type][y][x] == 3) {
                    pixels.push_back({ 3, Vec2 { x - PiecesCenters[type].x, y - PiecesCenters[type].y } });
                } else if (Pieces[type][y][x] == 4) {
                    pixels.push_back({ 4, Vec2 { x - PiecesCenters[type].x, y - PiecesCenters[type].y } });
                } else if (Pieces[type][y][x] == 5) {
                    pixels.push_back({ 5, Vec2 { x - PiecesCenters[type].x, y - PiecesCenters[type].y } });
                } else if (Pieces[type][y][x] == 6) {
                    pixels.push_back({ 6, Vec2 { x - PiecesCenters[type].x, y - PiecesCenters[type].y } });
                } else if (Pieces[type][y][x] == 7) {
                    pixels.push_back({ 7, Vec2 { x - PiecesCenters[type].x, y - PiecesCenters[type].y } });
                }
            }
        }

        // rotate pixels

        if (type != Square) {
            for (int i = 0; i < rotation; i++) { // rotate 90˚ (https://stackoverflow.com/questions/4780119/2d-euclidean-vector-rotations)
                for (auto& p : pixels) {
                    p.pos = Vec2 { -p.pos.y, p.pos.x };
                }
            }
        }

        // move pixels

        for (auto& p : pixels) {

            int32_t p_x = p.pos.x + PiecesCenters[type].x + position.x;

            if (_cfg_overflow_enable == true) {

                while (p_x < 0) {
                    p_x += _cfg_width;
                }
                while (p_x >= _cfg_width) {
                    p_x -= _cfg_width;
                }
            }
            p.pos = Vec2 { p_x, p.pos.y + PiecesCenters[type].y + position.y };
        }
    }
}