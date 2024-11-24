#include "esp_log.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include <Logic.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <cstdint>
#include <stdio.h>

#include "main.hpp"

// valcotris apps
#include "fire_anim/fire.hpp"
#include "shanke/shanke.hpp"
#include "tetris/tetris.hpp"

void device_state::enter_launcher() {
    while (true) {
        active_callback = &fire_anim::fire_input_cb;
        fire_anim::fire_main();

        active_callback = &shanke::shanke_input_cb;
        shanke::shanke_main();

        active_callback = &tetris::tetris_input_cb;
        tetris::tetris_main();
    }
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

static device_state dstate;

// called when receiving data from the joystick
static void recieve_callback(const esp_now_recv_info_t* info, const unsigned char* data, int data_len) {
    dstate.process_input(data);
}

int get_brightness() noexcept {
    return dstate.get_brightness();
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