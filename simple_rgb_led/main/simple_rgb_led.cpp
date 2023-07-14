#include <stdio.h>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "led_strip.h"
#include <string.h> //memset function
using namespace std;


//-----------------------------------  LED Stuff
static const char *TAG = "BLINK_EXAMPLE";
static led_strip_handle_t led_strip;

#define BLINK_GPIO 8  //PIN 8 is BuiltIn LED for ESP32-C3
static void configure_led(led_strip_handle_t* led_strip)
{
   ESP_LOGI(TAG, "Example configured to blink addressable LED!");
   /* LED strip initialization with the GPIO and pixels number*/
   led_strip_config_t strip_config;
   memset(&strip_config,0,sizeof(led_strip_config_t) );//Remplir la structure avec des valeurs vides
   strip_config.strip_gpio_num = BLINK_GPIO;
       strip_config.max_leds = 1; // at least one LED on board

   led_strip_rmt_config_t rmt_config;
   memset(&rmt_config,0, sizeof(led_strip_rmt_config_t) );
   rmt_config.resolution_hz = 10 * 1000 * 1000; // 10MHz

   ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, led_strip));
   /* Set all LED off to clear all pixels */
   led_strip_clear( (*led_strip) );
}



//2023-06-04 12:20:06 - Turn led ON
static void turn_RGB_led(led_strip_handle_t led_strip, uint8_t s_led_state,  uint32_t r=80, uint32_t g=80, uint32_t b=80)
{ 
   /* If the addressable LED is enabled */
   if (s_led_state) {
       /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
       led_strip_set_pixel(led_strip, 0, r,g,b);
       /* Refresh the strip to send data */
       led_strip_refresh(led_strip);
   } else {
       /* Set all LED off to clear all pixels */
       led_strip_clear(led_strip);
   }
}

//----------------------------------- /LED Stuff
static uint8_t s_led_state = 0;

extern "C" void app_main(void)
{


   printf("Configuring LED strip portTICK_PERIOD_MS=%d\n", (int)portTICK_PERIOD_MS);
   configure_led(&led_strip);

   s_led_state=1;       
   turn_RGB_led(led_strip,s_led_state /*,157,168,19*/);
   ESP_LOG_LEVEL(ESP_LOG_WARN, TAG, "%d - LED IS ON", s_led_state);
   vTaskDelay(4*1000/portTICK_PERIOD_MS);

   s_led_state=0;
   turn_RGB_led(led_strip,s_led_state);
   ESP_LOG_LEVEL(ESP_LOG_WARN, TAG, "%d - LED IS OFF", s_led_state);
   vTaskDelay(5*1000/portTICK_PERIOD_MS);



   cout<<" END PROGRAM "<<endl;
}
