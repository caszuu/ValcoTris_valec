#include "esp_log.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include <Logic.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <cstdint>
#include <stdio.h>

void device_state::enter_launcher() {

}

void device_state::process_input(const void* data) {
    joystick_event ev;
    ev.raw_input = *reinterpret_cast<const Vec3*>(data);

    if (next_valid_input_time > millis()) return;

    // process raw input

    ev.current_input = Vec3{
        (_cfg_joy_res - (ev.raw_input.z)) - (_cfg_joy_res / 2),
        (_cfg_joy_res - (ev.raw_input.y)) - (_cfg_joy_res / 2),
        (ev.raw_input.x) - (_cfg_joy_res / 2),
    };

    ev.dpad_state = (ev.current_input.y < -_cfg_joy_dead << 0) | // top
                    (ev.current_input.x < -_cfg_joy_dead << 1) | // right
                    (ev.current_input.y > _cfg_joy_dead << 2) |  // bottom
                    (ev.current_input.x > _cfg_joy_dead << 3);   // left

    // forward input event

    next_valid_input_time = millis() + _cfg_joy_cooldown;

    if (!active_callback) {
        printf("no active joystick callback!\n");
        return;
    }

    active_callback(ev);
}

/*
device input handling

extern void shanke_left();
extern void shanke_right();
extern void shanke_up();
extern void shanke_down();

extern void tetris_left();
extern void tetris_right();
extern void tetris_up();
extern void tetris_down();
extern void tetris_cw();
extern void tetris_ccw();

extern void tetris_x_mid();
extern void tetris_y_mid();
extern void tetris_z_mid();

extern void stop_fire();
*/

int brightness = 50;

bool GameOverBool = false;
bool Paused = false;

static device_state dstate;

// called when receiving data from the joystick
static void recieve_callback(const esp_now_recv_info_t* info, const unsigned char* data, int data_len) {
    dstate.process_input(data);
    return;

    printf("x:%d y:%d z:%d \n", data_s->x, data_s->y, data_s->z);

    int joy_x = (_cfg_joy_res - (data_s->z)) - (_cfg_joy_res / 2);
    int joy_y = (_cfg_joy_res - (data_s->y)) - (_cfg_joy_res / 2);
    int joy_z = (data_s->x) - (_cfg_joy_res / 2);

    if (joy_x < -_cfg_joy_dead) { // left
        switch (mode) {
        case 0:
            tetris_left();
            break;
        case 1:
            shanke_left();
            break;
        case 2:
            stop_fire();
            break;

        default:
            break;
        }
        cooldown_timer = millis() + cooldown;
    } else if (joy_x > _cfg_joy_dead) { // right
        switch (mode) {
        case 0:
            tetris_right();
            break;
        case 1:
            shanke_right();
            break;
        case 2:
            stop_fire();
            break;

        default:
            break;
        }
        cooldown_timer = millis() + cooldown;
    } else {
                switch (mode) {
        case 0:
            tetris_x_mid();
            break;
        case 1:
            break;
        case 2:
            break;

        default:
            break;
        }
        cooldown_timer = millis() + cooldown;
    }

    if (joy_y < -_cfg_joy_dead) { // up
        switch (mode) {
        case 0:
            tetris_up();
            break;
        case 1:
            shanke_up();
            break;
        case 2:
            stop_fire();
            break;

        default:
            break;
        }
        cooldown_timer = millis() + cooldown;
    } else if (joy_y > _cfg_joy_dead) { // down
        switch (mode) {
        case 0:
            tetris_down();
            break;
        case 1:
            shanke_down();
            break;
        case 2:
            stop_fire();
            break;

        default:
            break;
        }
        cooldown_timer = millis() + cooldown;
    } else {
        switch (mode) {
        case 0:
            tetris_y_mid();
            break;
        case 1:
            break;

        default:
            break;
        }
        cooldown_timer = millis() + cooldown;
    }

    if (joy_z < -_cfg_joy_dead) { // cw
        switch (mode) {
        case 0:
            tetris_cw();
            break;
        case 1:
            break;
        case 2:
            stop_fire();
            break;

        default:
            break;
        }
        cooldown_timer = millis() + cooldown;
    } else if (joy_z > _cfg_joy_dead) {
        switch (mode) {
        case 0:
            tetris_ccw();
            break;
        case 1:
            break;
        case 2:
            stop_fire();
            break;

        default:
            break;
        }
        cooldown_timer = millis() + cooldown;
    } else {
        switch (mode) {
        case 0:
            tetris_z_mid();
            break;
        case 1:
            break;
        case 2:
            break;

        default:
            break;
        }
        cooldown_timer = millis() + cooldown;
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

    // start launcher
    dstate.enter_launcher();
}