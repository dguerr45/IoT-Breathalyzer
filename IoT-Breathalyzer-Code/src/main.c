#include <stdio.h>

//freertos headers
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//GPIO header
#include "driver/gpio.h"

//LED header
#include "driver/ledc.h"

//ADC API headers
#include "esp_adc/adc_oneshot.h"

//Wifi header
#include "esp_wifi.h"

#define BUZZER_PIN      GPIO_NUM_32
#define RED_LED_PIN     GPIO_NUM_33
#define GREEN_LED_PIN   GPIO_NUM_25

#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_TIMER          LEDC_TIMER_0
#define RED_LEDC_CHANNEL    LEDC_CHANNEL_0
#define GREEN_LEDC_CHANNEL  LEDC_CHANNEL_1

void redLightTask(){
    while(true){
        ledc_set_duty(LEDC_MODE, RED_LEDC_CHANNEL, 255);  //on at full brightness
        ledc_update_duty(LEDC_MODE, RED_LEDC_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ledc_set_duty(LEDC_MODE, RED_LEDC_CHANNEL, 255/2);  //on at half brightness
        ledc_update_duty(LEDC_MODE, RED_LEDC_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ledc_set_duty(LEDC_MODE, RED_LEDC_CHANNEL, 0);  //off
        ledc_update_duty(LEDC_MODE, RED_LEDC_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void greenLightTask(){
    while(true){
        ledc_set_duty(LEDC_MODE, GREEN_LEDC_CHANNEL, 0);  //off
        ledc_update_duty(LEDC_MODE, GREEN_LEDC_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ledc_set_duty(LEDC_MODE, GREEN_LEDC_CHANNEL, 255/2);  //on at half brightness
        ledc_update_duty(LEDC_MODE, GREEN_LEDC_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ledc_set_duty(LEDC_MODE, GREEN_LEDC_CHANNEL, 255);  //on at full brightness
        ledc_update_duty(LEDC_MODE, GREEN_LEDC_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    // Replace with your actual Wi-Fi network name and password
    const char* ssid = "SSID_HERE";
    const char* password = "PASSWORD_HERE";

    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(RED_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GREEN_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_NUM_12, GPIO_MODE_INPUT);

    // Configure LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = 8,               // 8-bit (0-255)
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000                     //5kHz PMW frequency
    };
    ledc_timer_config(&ledc_timer);

    // Configure Red LEDC channel
    ledc_channel_config_t red_ledc_channel = {
        .gpio_num = RED_LED_PIN,
        .speed_mode = LEDC_MODE,
        .channel = RED_LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .duty = 0
    };
    ledc_channel_config(&red_ledc_channel);

    // Configure Green LEDC channel
    ledc_channel_config_t green_ledc_channel = {
        .gpio_num = GREEN_LED_PIN,
        .speed_mode = LEDC_MODE,
        .channel = GREEN_LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .duty = 0
    };
    ledc_channel_config(&green_ledc_channel);

    //creating tasks that should run until power off/reset
    xTaskCreate(redLightTask, "redLightTask function", 500, NULL, 1, NULL);
    xTaskCreate(greenLightTask, "greenLightTask function", 500, NULL, 1, NULL);
}