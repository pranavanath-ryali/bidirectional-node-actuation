#include "esp_err.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include <enc.h>
#include <servo.h>
#include <stdint.h>
#include <string.h>

int angle = 0;

void espnow_recv_cb(const esp_now_recv_info_t *info, const uint8_t *data, int data_len) {
    if (data_len == sizeof(int)) {
        int a;
        memcpy(&a, data, sizeof(a));

        angle = a;
    }
}

static inline void espnow_init(void) {
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    uint8_t mac_addr[8];
    esp_wifi_get_mac(WIFI_IF_STA, mac_addr);
    ESP_LOGI("MAC ADDR", "%02X:%02X:%02X:%02X:%02X:%02X", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    ESP_ERROR_CHECK(esp_now_init());
    esp_now_register_recv_cb(espnow_recv_cb);
}

void app_main(void) {
    espnow_init();
    servo_init();
    encoder_init();

    uint32_t prev_enc_pos = -1;
    uint32_t prev_angle = 0;

    servo_set_angle(prev_angle);

    while (1) {
        int32_t pos;
        portENTER_CRITICAL(&enc_mux);
        if (enc_position < 0) enc_position = 0;
        if (enc_position > MAX_POS) enc_position = MAX_POS;
        pos = enc_position;
        portEXIT_CRITICAL(&enc_mux);

        if (pos != prev_enc_pos) {
            prev_enc_pos = pos;
            angle = pos * ANGLE_STEP;
        }

        if (angle != prev_angle) {
            prev_angle = angle;
            servo_set_angle(angle);
            ESP_LOGI("SERVO", "Set angle: %d deg", angle);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
