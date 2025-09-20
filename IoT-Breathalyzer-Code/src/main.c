/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_wifi.h"

#define BUZZER      GPIO_NUM_32
#define RED_LED     GPIO_NUM_33
#define GREEN_LED   GPIO_NUM_25

#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_CHANNEL LEDC_CHANNEL_0



void app_main(void)
{
    // Replace with your actual Wi-Fi network name and password
    const char* ssid = "SSID_HERE";
    const char* password = "PASSWORD_HERE";

    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
    gpio_set_direction(RED_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(GREEN_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_NUM_12, GPIO_MODE_INPUT);

    const int ledChannel = 0;
    // Configure LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = 8,               // 8-bit (0-255)
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000                     //5kHz PMW frequency
    };
    ledc_timer_config(&ledc_timer);

    // Configure LEDC channel
    ledc_channel_config_t ledc_channel = {
        .gpio_num = RED_LED,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .duty = 0
    };
    ledc_channel_config(&ledc_channel);

    while(true){
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 255);  //on at full brightness
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 255/2);  //on at half brightness
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);  //off
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}