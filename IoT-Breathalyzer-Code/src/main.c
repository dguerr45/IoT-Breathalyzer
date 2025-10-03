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

void alcoholSensorTask(void* data){
    int* out_raw_GPIO2 = (int*) data;

    //Creating ADC Unit Handle
    adc_oneshot_unit_handle_t adc2_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc2_handle));

    //Creating ADC Channel to setup input pins
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,        //0-4095 output
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, ADC_CHANNEL_2, &config));  //Setting up GPIO pin 2

    while(true){
        //Reading input from GPIO Pin 2
        ESP_ERROR_CHECK(adc_oneshot_read(adc2_handle, ADC_CHANNEL_2, out_raw_GPIO2));

        vTaskDelay(pdMS_TO_TICKS(200));
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
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);        //Alcohol Sensor
    gpio_set_direction(GPIO_NUM_12, GPIO_MODE_INPUT);       //Heart Pulse Sensor

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
    //xTaskCreate(redLightTask, "redLightTask function", 500, NULL, 1, NULL);
    //xTaskCreate(greenLightTask, "greenLightTask function", 500, NULL, 1, NULL);

    //Creating task for Alcohol Sensor
    int out_raw_GPIO2;
    xTaskCreate(alcoholSensorTask, "alcoholSensorTask", 1024, &out_raw_GPIO2, 1, NULL);

    while(true){
        printf("The output from GPIO2 is: %d\n", out_raw_GPIO2);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}