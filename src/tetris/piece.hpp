#pragma once

#include "pieceTable.h"
#include "utils.hpp"

#include <vector>

namespace tetris {
    struct PiecePixel {
        char value;
        Vec2 pos;
    };

    class Piece {
    public:
        Piece(PieceKind type, Vec2 pos, const uint8_t& rot); // constructor (called when creating the class), using const & to avoid copies
        ~Piece(); // destructor (called when deleting the class)

        Piece() = delete; // delete the default construtor to make creating empty piece ilegal

        void getShape(std::vector<PiecePixel>& pixels) const { renderShape(pixels); }

        void lower(); // DOESN'T CHECK FOR COLLISION!
        void rotateClockwise();
        void rotateCounterClockwise();

        void confirm();
        void undo();

        void draw();

        bool getInteract() const { return lowerLast; }

        void interact() { lowerLast = false; }

        Vec2 position;
        uint8_t rotation = 0;
    private:
        void renderShape(std::vector<PiecePixel>& pixels) const; // positions, rotates and converts shape to pixels
        PieceKind type;

        Vec2 lastPosition;
        uint8_t lastRotation;

        bool lowerLast;
    };
}