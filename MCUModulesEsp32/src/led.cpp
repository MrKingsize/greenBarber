/** @file monitor.cpp
 * 
 * @brief File to control the inter modules communication monitor system. 
 *
 */ 

/****************************** Includes *************************************/
#include <Arduino.h>
#include "monitor.h"
#include "common.h"
#include "module_map.h"
#include "led.h"

/*************************** Global Variables ********************************/
volatile uint8_t activeState = STATE_INIT;

/****************************** Functions *************************************/

/******************************************************************************
 * @brief sets the current state of the module
 * state: new state
 *******************************************************************************/
void set_mon_state(uint8_t state)
{
    activeState = state;
}

/******************************************************************
 * @brief processes led blick state
 * @note has to be called periodically and the led state corresponds to the current module state
 *****************************************************************/
void led_ctrl()
{
    static uint8_t led_value_toggle = LOW;
    static uint8_t loopCounter = 0;

    switch (activeState)
	{
	case STATE_INIT:
		//DEBUG_PRINTF("Init led\n");
		digitalWrite(LED_PIN, HIGH);
		break;
	case STATE_COMMS_CONNECTED:
		if (loopCounter * TOTAL_MAIN_LOOP_DELAY >= LED_BLINK_INTERVAL_COMMS_CONNECTED_MS){
			digitalWrite(LED_PIN, led_value_toggle);
			led_value_toggle = !led_value_toggle;
			loopCounter = 0;
		}
		break;

	case STATE_COMMS_NOT_CONNECTED:
		if (loopCounter * TOTAL_MAIN_LOOP_DELAY >= LED_BLINK_INTERVAL_COMMS_NOT_CONNECTED_MS){
			DEBUG_PRINTF("Blick led COMMS_NOT_CONNECTED\n");
			digitalWrite(LED_PIN, led_value_toggle);
			led_value_toggle = !led_value_toggle;
			loopCounter = 0;			
		}
		break;
	
	default:
		break;
	}

    loopCounter++;
}

/******************************************************************
 * @brief initializes the led control system
 *****************************************************************/
void led_init(void)
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // off
}