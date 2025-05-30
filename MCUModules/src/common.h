/** @file common.h
 * 
 * @brief Header for the motors control. 
 *
 */ 
#ifndef COMMON_ENABLED
#define COMMON_ENABLED

/****************************** Includes *************************************/
#include <Arduino.h>

/****************************** Constants *************************************/

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

#endif