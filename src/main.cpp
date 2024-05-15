#include "esp_log.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include <Logic.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <stdio.h>

#include "config.hpp"

// Valec MAC    = C8:2B:96:B5:CA:84
// Joystick MAC = C8:2B:96:B8:C9:C0

int mode = 0;

extern int tetris_main();
extern int shanke_main();

extern void shanke_left();
extern void shanke_right();

extern void tetris_left();
extern void tetris_right();
extern void tetris_up();
extern void tetris_down();
extern void tetris_cw();
extern void tetris_ccw();

extern void tetris_x_mid();
extern void tetris_y_mid();
extern void tetris_z_mid();

long cooldown_timer = 0;
long cooldown = 250;

int brightness = 5;

bool GameOverBool = false;
bool Paused = false;

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
    if (cooldown_timer > millis()) {
        return;
    }

    printf("x:%d y:%d z:%d \n", data_s->x, data_s->y, data_s->z);

    int joy_x = (data_s->x) - (_cfg_joy_res / 2);
    int joy_y = (data_s->y) - (_cfg_joy_res / 2);
    int joy_z = (data_s->z) - (_cfg_joy_res / 2);

    if (joy_x < -_cfg_joy_dead) {
        switch (mode) {
        case 0:
            tetris_left();
            /* code */
            break;
        case 1:
            shanke_left();
            break;

        default:
            break;
        }
        cooldown_timer = millis() + cooldown;
    } else if (joy_x > _cfg_joy_dead) {
        switch (mode) {
        case 0:
            tetris_right();
            /* code */
            break;
        case 1:
            shanke_right();
            break;

        default:
            break;
        }
        cooldown_timer = millis() + cooldown;
    } else {
        tetris_x_mid();
        cooldown_timer = millis() + cooldown;
    }

    if (joy_y < -_cfg_joy_dead) {
        tetris_up();
        cooldown_timer = millis() + cooldown;
    } else if (joy_y > _cfg_joy_dead) {
        tetris_down();
        cooldown_timer = millis() + cooldown;
    } else {
        tetris_y_mid();
        cooldown_timer = millis() + cooldown;
    }

    if (joy_z < -_cfg_joy_dead) {
        tetris_cw();
        cooldown_timer = millis() + cooldown;
    } else if (joy_z > _cfg_joy_dead) {
        tetris_ccw();
        cooldown_timer = millis() + cooldown;
    } else {
        tetris_z_mid();
        cooldown_timer = millis() + cooldown;
    }
}

void gameOver(int score) {
    display.clear();
    display.fill(Rgb(255, 0, 10));
    if (score > 120) {
        display.fill(Rgb(255, 255, 255));
    }
    delay(2000);

    GameOverBool = false;
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
        mode = 0; // tetris
        gameOver(tetris_main());

        mode = 1; // shanke
        gameOver(shanke_main());
    }
}