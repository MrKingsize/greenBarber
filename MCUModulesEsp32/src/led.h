/** @file led.h
 * 
 * @brief Header to control the led blinking control. 
 *
 */ 

#ifndef LED_ENABLED
#define LED_ENABLED

/****************************** Includes *************************************/
#include <Arduino.h>

 /****************************** Constants *************************************/

#define LED_PIN 2 // Built-in LED pin for ESP32
#define LED_BLINK_INTERVAL_COMMS_CONNECTED_MS 1000
#define LED_BLINK_INTERVAL_COMMS_NOT_CONNECTED_MS 500


/****************************** Structures *************************************/



/****************************** Function Prototypes *************************************/
void led_init(void);
void set_mon_state(uint8_t state);
void led_ctrl();

#endif