#include <Logic.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <esp_now.h>
#include <WiFi.h>

// Valec MAC    = C8:2B:96:B5:CA:84
// Joystick MAC = C8:2B:96:B8:C9:C0


#include "colors.hpp"
#include "global_vars.hpp"
#include "map.hpp"
#include "piece.hpp"
#include "tetris_config.hpp"

Map map;
std::unique_ptr<Piece> activePiece; // needs the ability to be null

extern std::array<Rgb, 8> colors;
extern std::array<Rgb, 10> number_colors;

long current_millis = 0;
long prev_millis = 0;
long interval = 1000; // 750

bool GameOverBool = false;
bool ShouldRunGame = true;

const int GeneratorBagSize = 7;
int GeneratorPieceIndex = 0;
int GeneratorPieceList[GeneratorBagSize];

int player_score = 0;

int brightness = 15;
int view_offset = 0;


//Structure example to receive data
//Must match the sender structure
typedef struct data_struct {
  int x;
  int y;
  int z;
} data_struct;

//Create a struct_message called data
data_struct data;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("x: ");
  Serial.println(data.x);
  Serial.print("y: ");
  Serial.println(data.y);
    Serial.print("z: ");
  Serial.println(data.z);
  Serial.println();
}




void GameOver() {
    GameOverBool = true;

    // show last frame
    for (int x = 0; x < _cfg_width; x++) {
        for (int y = 0; y < _cfg_height; y++) {
            int p_x = x + view_offset;
            while (p_x < 0) {
                p_x += _cfg_width;
            }
            while (p_x >= _cfg_width) {
                p_x -= _cfg_width;
            }
            display.setColor(p_x, y, colors[map.placedPixels[x][y]]);
        }
    }
    display.show(brightness);
}

void displayScore() {
    if (player_score < 0) {
        player_score = 0;
    }
    if (player_score > 10000) { // probably dont need this, but just to be safe
        player_score = 0;
    }

    int score_digits[5] = { 0, 0, 0, 0, 0 };

    score_digits[4] = player_score % 10;
    score_digits[3] = (player_score / 10) % 10;
    score_digits[2] = (player_score / 100) % 10;
    score_digits[1] = (player_score / 1000) % 10;
    score_digits[0] = (player_score / 10000) % 10;
    for (int i = 0; i < 5; i++) {
        statusBar[i] = number_colors[score_digits[i]];
    }

    //std::cout << score_digits[0] << " "<< score_digits[1] << " "<< score_digits[2] << " "<< score_digits[3] << " "<< score_digits[4] << "\n";

    statusBar.show(brightness);
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

    activePiece = std::make_unique<Piece>(pieceKind, Vec2 { 4 - view_offset, 0 }, 0); // create a new piece
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
            if (p.pos.x >= _cfg_width) {
                fail = true;
            }
        }
    }

    for (auto& p : pixels) {
        // Down
        if (p.pos.y >= _cfg_height) {
            hitBottom = true;
        }
    }

    if ((fail == true && hitBottom == false) || (map.checkCollision(*activePiece) && !activePiece->getInteract())) {
        activePiece->undo();
    } else if ((fail == false && hitBottom == true) || (map.checkCollision(*activePiece) && activePiece->getInteract())) {
        activePiece->undo();
        interval = 750;
        map << *activePiece; // merge
        map.checkLines();
        newPiece();
    } else {

        activePiece->confirm();
    }

    for (int i = 0; i < _cfg_width; i++) {
        if (map.placedPixels[i][1] != 0) {
            GameOver();
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

    // Score logic
    if (map.full_lines > 0) {
        if (map.full_lines == 2) {
            player_score += 3;
        } else if (map.full_lines == 3) {
            player_score += 5;
        } else if (map.full_lines == 4) {
            player_score += 8;
        } else {
            player_score += 1;
        }
        displayScore();

        map.full_lines = 0;
    }
}

void logicMain() {
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

       /*
       switch (int(val)) {
        case 1: // rotate
            activePiece->rotateClockwise();
            activePiece->interact();
            ShouldRunGame = true;
            break;
        case 2: // right
            activePiece->position.x++;
            activePiece->interact();
            ShouldRunGame = true;
            break;
        case 3: // left
            activePiece->position.x--;
            activePiece->interact();
            ShouldRunGame = true;
            break;
        case 4: // fast_down
            interval = 100;
            ShouldRunGame = true;
            break;
        case 5: // slow_down
            interval = 750;
            ShouldRunGame = true;
            break;

        default:
            break;
        }
    });
       */

    while (true) {
        // Setup functions
        GameOverBool = false;

        player_score = 0;
        display.clear();
        displayScore();

        map.clearMap();
        generatePieces();
        newPiece();

        // The loops
        while (!GameOverBool) {
            loop();
        }

        // If is game over
        delay(500); // wait a bit for
        ShouldRunGame = false;

        // Wait until any button is pressed, then restart the loop
        while (!ShouldRunGame) {
            delay(100);
        }
    }
}