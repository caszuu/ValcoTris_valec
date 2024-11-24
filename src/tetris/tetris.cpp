#include <Logic.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <stdio.h>

// Valec MAC    = C8:2B:96:B5:CA:84
// Joystick MAC = C8:2B:96:B8:C9:C0

#include "config.hpp"
#include "utils.hpp"
#include "input.hpp"
#include "map.hpp"
#include "piece.hpp"

// with the amount of globals this should be a class.. 
namespace tetris {
    Map map;
    std::unique_ptr<Piece> activePiece; // needs the ability to be null

    long current_millis = 0;
    long prev_millis = 0;
    long interval = 1000; // 750

    bool GameOverBool = false;

    const int GeneratorBagSize = 7;
    int GeneratorPieceIndex = 0;
    int GeneratorPieceList[GeneratorBagSize];

    int player_score = 0;

    int view_offset = 0;
    int last_view_offset = 0;

    static void displayFrame() {
        if (!GameOverBool) {
            display.clear();
            map.draw();
            activePiece->draw();

            display.show(get_brightness());
        }
    }

    static void generatePieces() {
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

    static void newPiece() {
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

    void tetris_input_cb(const joystick_event& ev) {
        if (ev.is_dpad_active(dpad_direction::up)) {
            activePiece->rotateCounterClockwise();
            activePiece->interact();
        } else if (ev.is_dpad_active(dpad_direction::down)) {
            interval = 100;
        } else {
            interval = 750;
        }

        if (ev.is_dpad_active(dpad_direction::right)) {
            /*activePiece->rotateCounterClockwise();
            activePiece->interact();*/
            activePiece->position.x--;
            last_view_offset = view_offset;
            view_offset++;

            activePiece->interact();
        } else if (ev.is_dpad_active(dpad_direction::left)) {
            /*activePiece->rotateCounterClockwise();
            activePiece->interact();*/
            activePiece->position.x++;
            last_view_offset = view_offset;
            view_offset--;

            activePiece->interact();
        }

        if (ev.current_input.z < -_cfg_joy_dead) {
            activePiece->rotateClockwise();
            activePiece->interact();
        } else if (ev.current_input.z > _cfg_joy_dead) {
            activePiece->rotateCounterClockwise();
            activePiece->interact();
        }
    }

    int tetris_main() {
        player_score = 0;
        display.clear();

        map.clearMap();
        generatePieces();
        newPiece();

        while (!GameOverBool) {
            current_millis = millis();
            if (current_millis - prev_millis >= interval) {
                activePiece->lower();
                prev_millis = current_millis;
            }

            test();
            displayFrame();
        }

        return player_score;
    }
}