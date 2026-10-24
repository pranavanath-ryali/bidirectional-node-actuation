#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_now.h"

uint8_t nodes_mac[1][6] = {
    // { 0x14, 0x63, 0x93, 0xB3, 0x2E, 0xCC }
    { 0x44, 0xB1, 0x76, 0x16, 0xF3, 0xB0 }
};

void app_main(void) {
    printf("INITIALIZING HUB INTERFACE: S3\n");
    printf("MAC ADDRESS OF NODES:\n");
    for (int i = 0; i < sizeof(nodes_mac) / sizeof(nodes_mac[0]); i++) {
        printf("MAC ADDRESS: %02X:%02X:%02X:%02X:%02X:%02X\n",
                   nodes_mac[i][0], nodes_mac[i][1], nodes_mac[i][2], nodes_mac[i][3], nodes_mac[i][4], nodes_mac[i][5]);
    }

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    if (esp_now_init() != ESP_OK) {
        printf("Error initializing ESP-NOW\n");
        return;
    }

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, nodes_mac[0], 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        printf("Failed to add peer\n");
        return;
    }

    int angle = 0;
    int direction = 1;
    while (1) {
        ESP_LOGI("ESPNOW", "Send angle: %d", angle);
        esp_err_t err = esp_now_send(nodes_mac[0], (uint8_t*)&angle, sizeof(angle));
        if (err != ESP_OK) {
            ESP_LOGI("ESPNOW", "Couldn't send data");
        }

        angle += 15 * direction;
        if (angle < 0) {
            angle = 0;
            direction = 1;
        }
        if (angle > 180) {
            angle = 180;
            direction = -1;
        }

        // snprintf(message, sizeof(message), "HELLO C3 #%d", count++);

        // printf("Sending Data\n");
        // esp_err_t result = esp_now_send(nodes_mac[0], (uint8_t*)message, strlen(message) + 1);
        // if (result != ESP_OK) {
        //     printf("Error sending data\n");
        // }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
