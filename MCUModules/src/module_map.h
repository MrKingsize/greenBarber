/** @file module_map.h
 * 
 * @brief Header for the modules configuration mapping. 
 *
 */ 

 
#ifndef MODULE_MAP_ENABLED
#define MODULE_MAP_ENABLED

/****************************** Includes *************************************/
#include <Arduino.h>
#include "motor.h"

/****************************** Constants *************************************/

#define MODULE_NUM sizeof(moduleMap) / sizeof(moduleMap[0])

/******************************************************************************
 * CAN Commands Definition
 ******************************************************************************/
// Individual HW modules commands list
// If adding new cmds add also in moduleCmdMap
typedef enum
{
    CMD_SET_DIR_ANGLE_RIGHT,
    CMD_READ_DIR_ANGLE_RIGHT,
    CMD_CALIBRATE_DIR_RIGHT,
    CMD_SET_DIR_ANGLE_LEFT,
    CMD_READ_DIR_ANGLE_LEFT,
    CMD_CALIBRATE_DIR_LEFT,
    CMD_SET_TRACTION_SPEED_LEFT,
    CMD_SET_X_TICK,
    CMD_READ_X_TICK,
    CMD_CALIBRATE_X,
    CMD_SET_Y_TICK,
    CMD_READ_Y_TICK,
    CMD_CALIBRATE_Y,
    CMD_SET_Z_TICK,
    CMD_READ_Z_TICK,
    CMD_CALIBRATE_Z,
    CMD_TURN_ON_CUT,
    CMD_TURN_OFF_CUT,
} CMD_HW_ENUM_E;

// General High level commands from Jetson to main MCU
// If adding new cmds add also in moduleCmdMap
typedef enum
{
    // 3 axis bridge commands list
    CMD_SET_COORDINATE_X = 1,  // Set coordinate X
    CMD_SET_COORDINATE_Y = 2,  // Set coordinate Y
    CMD_HARVEST = 3,           // Harvest
    CMD_CALIBRATE_XY = 4,         // Calibrate
    READ_COORDINATES_X = 5,    // Read coordinates X
    READ_COORDINATES_Y = 6,    // Read coordinates Y
    READ_INTERNAL_STATE_XY = 7,   // Read internal state of 3-axis-bridge axis

    // traction module commands list
    CMD_MOVE_FORWARD = 20,     // Move forward
    CMD_MOVE_BACKWARDS = 21,   // Move backwards
    CMD_STOP = 22,             // Stop
    READ_TRACTION_STATE_TRACTION = 23,  // Read internal state traction module

    // direction module commands list
    CMD_SET_DIRECTION_ANGLE = 30,  // Set direction angle
    CMD_CALIBRATE_DIRECTION = 31,  // Calibrate
    READ_DIRECTION_STATE_DIRECTION = 32,     // Read internal state direction module

    // power modules commands list
    READ_POWER_STATUS = 40      // Read power status
} CMD_MCU_MAIN_MODULE_ENUM_E;

typedef enum
{
    ERROR_ACK = 0,                     // Command ACK
    ERROR_INVALID_MODULE = 10,         // Invalid Module
    ERROR_INVALID_COMMAND = 11,        // Invalid Command
    ERROR_PAYLOAD_OUT_OF_RANGE = 12    // Payload Out Of Supported Range
} ERROR_CODE_ENUM_E;

const uint8_t MCU_MAIN_CMDS[] PROGMEM = {
    CMD_SET_COORDINATE_X, 
    CMD_SET_COORDINATE_Y, 
    CMD_HARVEST, 
    CMD_CALIBRATE_XY,
    READ_COORDINATES_X, 
    READ_COORDINATES_Y, 
    READ_INTERNAL_STATE_XY,
    CMD_MOVE_FORWARD, 
    CMD_MOVE_BACKWARDS, 
    CMD_STOP, 
    READ_TRACTION_STATE_TRACTION,
    CMD_SET_DIRECTION_ANGLE, 
    CMD_CALIBRATE_DIRECTION, 
    READ_DIRECTION_STATE_DIRECTION, 
    READ_POWER_STATUS
};

const uint8_t HW_DIR_RIGHT_CMDS[] PROGMEM = {
    CMD_SET_DIR_ANGLE_RIGHT, 
    CMD_READ_DIR_ANGLE_RIGHT, 
    CMD_CALIBRATE_DIR_RIGHT
};

const uint8_t HW_DIR_LEFT_CMDS[] PROGMEM = {
    CMD_SET_DIR_ANGLE_LEFT, 
    CMD_READ_DIR_ANGLE_LEFT, 
    CMD_CALIBRATE_DIR_LEFT
};

const uint8_t HW_X_TRACTION_LEFT_CMDS[] PROGMEM = {
    CMD_SET_X_TICK, 
    CMD_READ_X_TICK, 
    CMD_CALIBRATE_X, 
    CMD_SET_TRACTION_SPEED_LEFT
};

const uint8_t HW_Y_CMDS[] PROGMEM = {
    CMD_SET_Y_TICK, 
    CMD_READ_Y_TICK, 
    CMD_CALIBRATE_Y
};

const uint8_t HW_Z_CMDS[] PROGMEM = {
    CMD_SET_Z_TICK, 
    CMD_READ_Z_TICK, 
    CMD_CALIBRATE_Z
};

const uint8_t HW_CUT_CMDS[] PROGMEM = {
    CMD_TURN_ON_CUT, 
    CMD_TURN_OFF_CUT
};


/******************************************************************************
 * Module maps Definition
 ******************************************************************************/

// Modules selection menu for module byte. 
// Continuity in the Indexes values must be followed
typedef enum
{
    MOD_SEL_JETSON = 0,             // Module selection for Jetson Nano
    MOD_SEL_MCU_MAIN = 1,           // Main MCU module, traction right and turbines
    MOD_SEL_HW_DIR_RIGHT = 2,       // Individual HW module direction right side
    MOD_SEL_HW_DIR_LEFT = 3,        // Individual HW module direction left side
    MOD_SEL_HW_X_TRACTION_LEFT = 4, // Individual HW module X axis and traction left side
    MOD_SEL_HW_Y = 5,               // Individual HW module Y axis
    MOD_SEL_HW_Z = 6,               // Individual HW module Z axis
    MOD_SEL_HW_CUT = 7              // Individual HW module harvesting
} MOD_SEL_ENUM_E;


/****************************** Structures *************************************/


// Fixed CAN and Communication parameters
struct module_cmd_t
{
    const MOD_SEL_ENUM_E moduleIdx; // Module selection from MOD_SEL_ENUM_E
    const uint8_t moduleID;         // Module ID is both the can ID and the ID stored in flash of each module, it must correspond to (moduleIdx + 1)
    const uint8_t *cmdList;         // pointer to array stored in flash with command list
    const uint8_t cmdListCount;     // commands list count
    const uint8_t *motorsList;      // pointer to array stored in flash with motors list
    const uint8_t motorsCount;      // motors list count
};


/**********************************************************************
 * store fixed parameters in flash (PROGMEM)
 * parameters access information, use:
 * uint8_t -> pgm_read_byte(&moduleMap[0].moduleID)
 * uint16_t -> pgm_read_word()
 * uint32_t -> pgm_read_dword()
 * pointer -> const struct module_cmd_t *mod = &moduleMap[index]; // Get the struct
 * const uint8_t *cmdListPtr = (const uint8_t *)pgm_read_ptr(&mod->cmdList);
 * uint8_t cmdCount = pgm_read_byte(&mod->cmdListCount)
 * pgm_read_byte(&cmdListPtr[0])
 * ********************************************************************/
const struct module_cmd_t moduleMap[] PROGMEM =
{   //moduleIdx,    moduleCANID,    cmdList,    cmdListCount
    {MOD_SEL_JETSON,          1,    nullptr,                    0,                                                                      },
    {MOD_SEL_MCU_MAIN,        2,    MCU_MAIN_CMDS,              sizeof(MCU_MAIN_CMDS) / sizeof(MCU_MAIN_CMDS[0]), \
                                    MCU_MAIN_MOTORS,            sizeof(MCU_MAIN_MOTORS) / sizeof(MCU_MAIN_MOTORS[0])},
    {MOD_SEL_HW_DIR_RIGHT,    3,    HW_DIR_RIGHT_CMDS,          sizeof(HW_DIR_RIGHT_CMDS) / sizeof(HW_DIR_RIGHT_CMDS[0]), \
                                    HW_DIR_RIGHT_MOTORS,        sizeof(HW_DIR_RIGHT_MOTORS) / sizeof(HW_DIR_RIGHT_MOTORS[0])},
    {MOD_SEL_HW_DIR_LEFT,     4,    HW_DIR_LEFT_CMDS,           sizeof(HW_DIR_LEFT_CMDS) / sizeof(HW_DIR_LEFT_CMDS[0]), \
                                    HW_DIR_LEFT_MOTORS,         sizeof(HW_DIR_LEFT_MOTORS) / sizeof(HW_DIR_LEFT_MOTORS[0])},
    {MOD_SEL_HW_X_TRACTION_LEFT, 5, HW_X_TRACTION_LEFT_CMDS,    sizeof(HW_X_TRACTION_LEFT_CMDS) / sizeof(HW_X_TRACTION_LEFT_CMDS[0]), \
                                    HW_X_TRACTION_LEFT_MOTORS,  sizeof(HW_X_TRACTION_LEFT_MOTORS) / sizeof(HW_X_TRACTION_LEFT_MOTORS[0])},
    {MOD_SEL_HW_Y,            6,    HW_Y_CMDS,                  sizeof(HW_Y_CMDS) / sizeof(HW_Y_CMDS[0]), \
                                    HW_Y_MOTORS,                sizeof(HW_Y_MOTORS) / sizeof(HW_Y_MOTORS[0])},
    {MOD_SEL_HW_Z,            7,    HW_Z_CMDS,                  sizeof(HW_Z_CMDS) / sizeof(HW_Z_CMDS[0]), \
                                    HW_Z_MOTORS,                sizeof(HW_Z_MOTORS) / sizeof(HW_Z_MOTORS[0])},
    {MOD_SEL_HW_CUT,          8,    HW_CUT_CMDS,                sizeof(HW_CUT_CMDS) / sizeof(HW_CUT_CMDS[0]), \
                                    HW_CUT_MOTORS,              sizeof(HW_CUT_MOTORS) / sizeof(HW_CUT_MOTORS[0])}
};

/****************************** Function Prototypes *************************************/
void init_my_module_id(void);
uint8_t get_my_module_id(void);
void get_motors_list(uint8_t *motorList, uint8_t *motorsCount);

#endif