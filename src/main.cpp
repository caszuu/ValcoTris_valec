#include "esp_log.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include <Logic.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <stdio.h>

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

int view_offset = 0;

int brightness = 5;

//Structure example to receive data
//Must match the sender structure
typedef struct data_struct {
    int x;
    int y;
    int z;
} data_struct;

//Create a struct_message called data
data_struct data;

// void receive_callback(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
void recieve_callback(const esp_now_recv_info_t* info, const unsigned char* data, int data_len) {
    const data_struct* data_s = (const data_struct*)data;
    printf("x:%d y:%d z:%d \n", data_s->x, data_s->y, data_s->z);

    int joy_x = (data_s->x) - (_cfg_joy_res / 2);
    int joy_y = (data_s->y) - (_cfg_joy_res / 2);
    int joy_z = (data_s->z) - (_cfg_joy_res / 2);

    if (joy_x < -_cfg_joy_dead) {
        activePiece->position.x++;
        activePiece->interact();
        ShouldRunGame = true;
    } else if (joy_x > _cfg_joy_dead) {
        activePiece->position.x--;
        activePiece->interact();
        ShouldRunGame = true;
    }
    if (joy_y < -_cfg_joy_dead) {
        activePiece->rotateClockwise();
        activePiece->interact();
        ShouldRunGame = true;
    } else if (joy_y > _cfg_joy_dead) {
        interval = 100;
        ShouldRunGame = true;
    } else {
        interval = 750;
        ShouldRunGame = true;
    }
    if (joy_z < -_cfg_joy_dead) {
        activePiece->rotateCounterClockwise();
        activePiece->interact();
        ShouldRunGame = true;
    } else if (joy_z > _cfg_joy_dead) {
        activePiece->rotateClockwise();
        activePiece->interact();
        ShouldRunGame = true;
    }
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
}

void gameOver() {
    GameOverBool = true;

    // show last frame
    for (int x = 0; x < display.width(); x++) {
        for (int y = 0; y < display.height(); y++) {
            int p_x = x + view_offset;
            while (p_x < 0) {
                p_x += display.width();
            }
            while (p_x >= display.width()) {
                p_x -= display.width();
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
    } else if ((fail == false && hitBottom == true) || (map.checkCollision(*activePiece) && activePiece->getInteract())) {
        activePiece->undo();
        interval = 750;
        map << *activePiece; // merge
        map.checkLines();
        newPiece();
    } else {

        activePiece->confirm();
    }

    for (int i = 0; i < display.width(); i++) {
        if (map.placedPixels[i][1] != 0) {
            gameOver();
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

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Initialize ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());

    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    ESP_LOGI("main", "MAC: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    // Register callback function for receiving data
    esp_now_register_recv_cb(recieve_callback);

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