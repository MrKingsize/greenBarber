/** @file monitor.h
 * 
 * @brief Header to control the inter modules communication monitor system. 
 *
 */ 

#ifndef MONITOR_ENABLED
#define MONITOR_ENABLED

/****************************** Includes *************************************/
#include <Arduino.h>

 /****************************** Constants *************************************/
#define PING_INTERVAL_MS 1000
#define PING_TIMEOUT_MS 3000

// Module state machine corresponds to the module Led state
typedef enum{
  STATE_INIT = 0,
  STATE_COMMS_NOT_CONNECTED = 1,
  STATE_COMMS_CONNECTED = 2,
}STATE_ENUM;

/****************************** Structures *************************************/

struct modulestate_t{
    uint8_t moduleState;
    uint8_t previousModuleState;
    uint16_t last_ping_time_ms;        // in milliseconds
};

/****************************** Function Prototypes *************************************/

void set_mod_mon_state(uint8_t state, uint8_t moduleId);
uint8_t mon_ctrl(void);
void mon_init(void);

#endif