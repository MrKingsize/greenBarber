/** @file spiLib.h
 * 
 * @brief Header file for spi libs. 
 *
 */

#ifndef SPILIB_ENABLED
#define SPILIB_ENABLED

/****************************** Includes *************************************/

#include <Arduino.h>
#include <SPI.h>

/*********************** Constants *******************************/

#define MISO_PIN    MISO // MISO (Master In Slave Out): Pin 50
#define MOSI_PIN    51 // MOSI (Master Out Slave In): Pin 51
#define SCK_PIN     52 // SCK (Serial Clock): Pin 52
#define SS_PIN      53 // SS (Slave Select): Pin 53 (or any other pin you define)

// SPI Connections info
// Arduino Mega MISO (Pin 50) → Jetson Nano MISO (Pin 21)
// Arduino Mega MOSI (Pin 51) → Jetson Nano MOSI (Pin 19)
// Arduino Mega SCK (Pin 52) → Jetson Nano SCK (Pin 23)
// Arduino Mega SS (Pin 53) → Jetson Nano SS (Pin 24) (or any GPIO)


#define MESSAGE_LENGTH 4  // Define how many bytes per message

typedef enum
{
    MOD_SEL_3_AXIS_BRIDGE = 1,  // Module selection for 3-axis bridge
    MOD_SEL_TRACTION = 2,       // Module selection for traction
    MOD_SEL_DIRECTION = 3,      // Module selection for direction
    MOD_SEL_POWER = 4           // Module selection for power
} MOD_SEL_ENUM_E;


typedef enum
{
    CMD_SET_COORDINATE_X = 1,  // Set coordinate X
    CMD_SET_COORDINATE_Y = 2,  // Set coordinate Y
    CMD_HARVEST = 3,           // Harvest
    CMD_CALIBRATE = 4,         // Calibrate
    READ_COORDINATES_X = 10,   // Read coordinates X
    READ_COORDINATES_Y = 11,   // Read coordinates Y
    READ_INTERNAL_STATE = 12   // Read internal state
} CMD_3_AXIS_BRIDGE_ENUM_E;

typedef enum
{
    CMD_MOVE_FORWARD = 1,    // Move forward
    CMD_MOVE_BACKWARDS = 2,  // Move backwards
    CMD_STOP = 3,            // Stop
    READ_TRACTION_STATE = 4  // Read internal state
} CMD_TRACTION_ENUM_E;

typedef enum
{
    CMD_SET_DIRECTION_ANGLE = 1,  // Set direction angle
    CMD_CALIBRATE_DIRECTION = 2,  // Calibrate
    READ_DIRECTION_STATE = 3      // Read internal state
} CMD_DIRECTION_ENUM_E;

typedef enum
{
    READ_POWER_STATUS = 1  // Read power status
} CMD_POWER_ENUM_E;

typedef enum
{
    ERROR_ACK = 1,                     // Command ACK
    ERROR_INVALID_MODULE = -1,         // Invalid Module
    ERROR_INVALID_COMMAND = -2,        // Invalid Command
    ERROR_PAYLOAD_OUT_OF_RANGE = -3    // Payload Out Of Supported Range
} ERROR_CODE_ENUM_E;




#endif /* SPILIB_ENABLED */