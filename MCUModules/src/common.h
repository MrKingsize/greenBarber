/** @file common.h
 * 
 * @brief Header for the motors control. 
 *
 */ 
#ifndef COMMON_ENABLED
#define COMMON_ENABLED

/****************************** Includes *************************************/
#include <Arduino.h>
#include "iso-tp.h"

/****************************** Constants *************************************/
#define CAN_ANGLE_OFFSET 100
#define MAIN_LOOP_DELAY 50 // ms
#define TOTAL_MAIN_LOOP_DELAY (MAIN_LOOP_DELAY + TIMEOUT_SESSION)// ms

//#define MODULE_ID_MOD_SEL_MCU_MAIN
//#define MODULE_ID_MOD_SEL_HW_DIR_RIGHT
#define MODULE_ID_MOD_SEL_HW_DIR_LEFT
//#define MODULE_ID_MOD_SEL_HW_X_TRACTION_LEFT

#define DEBUG_PRINTF(...)                     \
  do {                                        \
    char buf[128];                            \
    snprintf(buf, sizeof(buf), __VA_ARGS__);  \
    Serial.print(buf);                        \
    Serial.flush();                           \
  } while (0)

#define TRACE_PRINTF(...)                         \
  do {                                            \
    char buf[128];                                \
    snprintf(buf, sizeof(buf), "[%s] ", __func__);\
    Serial.print(buf);                            \
    snprintf(buf, sizeof(buf), __VA_ARGS__);      \
    Serial.print(buf);                            \
  } while (0)

void debug_counter_increase(void);

#endif