#include <Logic.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <stdio.h>

// Valec MAC    = C8:2B:96:B5:CA:84
// Joystick MAC = C8:2B:96:B8:C9:C0

#include "colors.hpp"
#include "config.hpp"
#include "global_vars.hpp"
#include "map.hpp"
#include "piece.hpp"

Map map;
std::unique_ptr<Piece> activePiece; // needs the ability to be null

extern std::array<Rgb, 8> colors;
extern std::array<Rgb, 10> number_colors;

extern void gameOver(int);
extern int brightness;

long current_millis = 0;
long prev_millis = 0;
long interval = 1000; // 750

extern bool GameOverBool;
extern bool Paused;

const int GeneratorBagSize = 7;
int GeneratorPieceIndex = 0;
int GeneratorPieceList[GeneratorBagSize];

int player_score = 0;

int view_offset = 0;
int last_view_offset = 0;

void tetris_up() {
    activePiece->rotateCounterClockwise();
    activePiece->interact();
}
void tetris_down() {
    interval = 100;
}
void tetris_y_mid() {
    interval = 750;
}


void tetris_left() {
    /*activePiece->rotateCounterClockwise();
    activePiece->interact();*/
    activePiece->position.x++;
    last_view_offset = view_offset;
    view_offset--;

    activePiece->interact();
}
void tetris_right() {
    /*activePiece->rotateCounterClockwise();
    activePiece->interact();*/
    activePiece->position.x--;
    last_view_offset = view_offset;
    view_offset++;
}
void tetris_x_mid() {
}


void tetris_cw() {
    activePiece->rotateClockwise();
    activePiece->interact();
    
}
void tetris_ccw() {
    activePiece->rotateCounterClockwise();
    activePiece->interact();
    

    activePiece->interact();
}
void tetris_z_mid() {
}

void displayFrame() {
    if (!GameOverBool) {
        display.clear();
        map.draw();
        activePiece->draw();

        display.show(brightness);
    }
}

void generatePieces() {
    for (int i = 0; i < GeneratorBagSize; i++) {
        GeneratorPieceList[i] = i;
    }

    for (int i = GeneratorBagSize - 1; i > 0; i--) {
        int j = random(0, i + 1);
        int temp = GeneratorPieceList[i];
        GeneratorPieceList[i] = GeneratorPieceList[j];
        GeneratorPieceList[j] = temp;
    }
    //std::cout << GeneratorPieceList[0] << " " << GeneratorPieceList[1] << " " << GeneratorPieceList[2] << " " << GeneratorPieceList[3] << " " << GeneratorPieceList[4] << " " << GeneratorPieceList[5] << " " << GeneratorPieceList[6] << "\n";
}

void newPiece() {
    GeneratorPieceIndex += 1;

    if (GeneratorPieceIndex >= GeneratorBagSize) {
        GeneratorPieceIndex = 0;
        generatePieces();
    }

    //std::cout << GeneratorPieceList[GeneratorPieceIndex] << "\n";
    PieceKind pieceKind = static_cast<PieceKind>(GeneratorPieceList[GeneratorPieceIndex]);

    activePiece = std::make_unique<Piece>(pieceKind, Vec2 { (int(_cfg_width / 2) - 1) - view_offset, 0 }, 0); // create a new piece
}

void test() {
    bool fail = false;
    bool hitBottom = false;

    std::vector<PiecePixel> pixels {};
    activePiece->getShape(pixels);

    if (_cfg_overflow_enable == false) {

        for (auto& p : pixels) {
            // Left
            if (p.pos.x < 0) {
                fail = true;
            }
            // Right
            if (p.pos.x >= display.width()) {
                fail = true;
            }
        }
    }

    for (auto& p : pixels) {
        // Down
        if (p.pos.y >= display.height()) {
            hitBottom = true;
        }
    }

    if ((fail == true && hitBottom == false) || (map.checkCollision(*activePiece) && !activePiece->getInteract())) {
        activePiece->undo();
        view_offset = last_view_offset;
    } else if ((fail == false && hitBottom == true) || (map.checkCollision(*activePiece) && activePiece->getInteract())) {
        activePiece->undo();
        view_offset = last_view_offset;
        interval = 750;
        map << *activePiece; // merge
        map.checkLines();
        newPiece();
    } else {

        activePiece->confirm();
        last_view_offset = view_offset;
    }

    for (int i = 0; i < display.width(); i++) {
        if (map.placedPixels[i][1] != 0) {
            GameOverBool = true;
        }
    }
}

void loop() {
    current_millis = millis();
    if (current_millis - prev_millis >= interval) {
        activePiece->lower();
        prev_millis = current_millis;
    }

    test();
    displayFrame();
}

int tetris_main() {
    player_score = 0;
    display.clear();

    map.clearMap();
    generatePieces();
    newPiece();

    // The loops
    while (!GameOverBool) {
        loop();
    }
    return player_score;
}