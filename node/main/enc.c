#include "freertos/FreeRTOS.h"
#include <stdio.h>
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "driver/gpio.h"

#include <enc.h>

volatile int32_t enc_position = 0;
portMUX_TYPE enc_mux = portMUX_INITIALIZER_UNLOCKED;

const int8_t qtable[16] = {
     0, +1, -1,  0,
    -1,  0,  0, +1,
    +1,  0,  0, -1,
     0, -1, +1,  0,
};
uint8_t prev_state = 0;
int8_t accum = 0;

void encoder_init(void) {
    gpio_config_t enc_cfg = {
        .pin_bit_mask = (1ULL << ENC_GPIO_A) | (1ULL << ENC_GPIO_B),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    ESP_ERROR_CHECK(gpio_config(&enc_cfg));

    prev_state = (gpio_get_level(ENC_GPIO_A) << 1 | gpio_get_level(ENC_GPIO_B));

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(ENC_GPIO_A, encoder_isr, NULL));
    ESP_ERROR_CHECK(gpio_isr_handler_add(ENC_GPIO_B, encoder_isr, NULL));
}

void IRAM_ATTR encoder_isr(void *arg) {
    uint8_t state = (gpio_get_level(ENC_GPIO_A) << 1) | gpio_get_level(ENC_GPIO_B);
    uint8_t idx = (prev_state << 2) | state;
    prev_state = state;

    portENTER_CRITICAL_ISR(&enc_mux);
    accum += qtable[idx];
    if (accum >= COUNTS_PER_DETENT) {
        enc_position++;
        accum = 0;
    } else if (accum <= -COUNTS_PER_DETENT) {
        enc_position--;
        accum = 0;
    }
    portEXIT_CRITICAL_ISR(&enc_mux);
}
