#ifndef NODE_ENC_H
#define NODE_ENC_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ENC_GPIO_A        GPIO_NUM_4
#define ENC_GPIO_B        GPIO_NUM_5
#define COUNTS_PER_DETENT 2

extern volatile int32_t enc_position;
extern portMUX_TYPE enc_mux;

void encoder_init(void);
void IRAM_ATTR encoder_isr(void *arg);

#endif
