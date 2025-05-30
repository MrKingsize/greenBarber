/** @file motor.h
 * 
 * @brief Header for the motors control. 
 *
 */ 
#ifndef MOTOR_ENABLED
#define MOTOR_ENABLED

/****************************** Includes *************************************/
#include <Arduino.h>

/****************************** Constants *************************************/

// rotation speed - choose only multiples of P_BASE.
#define P_BASE             40 // 40 us
//#define MOTOR_LAST         (sizeof(motor_config) / sizeof(motor_config[0]))
#define MAX_MOTORS_PER_MODULE   6

#define INTERNAL_WHEEL_FACTOR 0.8928
#define EXTERNAL_WHEEL_FACTOR 1.0768
#define CAN_ANGLE_OFFSET 100

// Available motor operations
typedef enum{
    MOTOR_STOP_CMD = 0,
    MOTOR_PLUS_CMD = 1,
    MOTOR_MINUS_CMD = 2,
}MOTOR_CONTROL;

// Motors list Idx
typedef enum{
    MOTOR_DIR_RIGHT = 0,
    MOTOR_DIR_LEFT,
    MOTOR_TRACTION_RIGHT,
    MOTOR_TRACTION_LEFT,
    MOTOR_X,
    MOTOR_Y,
    MOTOR_Z,
    MOTOR_ROTATE,
    MOTOR_CUT,
    MOTOR_TURBINES,
    MOTOR_GATE_1,
    MOTOR_GATE_2,
    MOTOR_LAST,
}MOTOR_ENUM;

// Motor types list
typedef enum{
    PWM_MOTOR = 0,
    STEPPER_MOTOR,
    STEPPER_PID,
}MOTOR_TYPES;


const uint8_t MCU_MAIN_MOTORS[] PROGMEM = {
    MOTOR_TRACTION_RIGHT,
    MOTOR_TURBINES,
    MOTOR_GATE_1,
    MOTOR_GATE_2
};

const uint8_t HW_DIR_RIGHT_MOTORS[] PROGMEM = {
    MOTOR_DIR_RIGHT
};

const uint8_t HW_DIR_LEFT_MOTORS[] PROGMEM = {
    MOTOR_DIR_LEFT
};

const uint8_t HW_X_TRACTION_LEFT_MOTORS[] PROGMEM = {
    MOTOR_TRACTION_LEFT,
    MOTOR_X
};

const uint8_t HW_Y_MOTORS[] PROGMEM = {
    MOTOR_Y
};

const uint8_t HW_Z_MOTORS[] PROGMEM = {
    MOTOR_Z
};

const uint8_t HW_CUT_MOTORS[] PROGMEM = {
    MOTOR_ROTATE,
    MOTOR_CUT
};

// Fixed motor parameters 
struct motor_config_t
{
    const MOTOR_TYPES motorType;                        // Motor type
    const uint32_t limit;                               // End limit of the motor, 0 = infinite
    const uint16_t defaultSpeedForward;                 // default speed which the motor moves forwards
    const uint16_t defaultSpeedBackwards;               // default speed which the motor moves backwards
    const MOTOR_ENUM motorIdx;                          // Modor Idx MOTOR_ENUM
    const uint8_t dir_or_rpwm_pin;                      // Driver direction pin or Right PWM pin
    const uint8_t pul_or_lpwm_pin;                      // Driver Pulse pin or Left PWM pin
    const uint8_t en_pin;                               // Driver Enable pin
    const uint8_t calib_pin;                            // End limit pin, 0 = no pin
    const uint8_t relay_pin;                            // Power Supply relay pin
    const uint8_t orientation;                          // dir value which pulses increment position                   
};


/**********************************************************************
 * store fixed parameters in flash (PROGMEM)
 * Note: parameter motorIdx has to allways be by order and continuously according to MOTOR_ENUM
 * parameters access information, use:
 * uint8_t -> pgm_read_byte(&motor_config[0].dir_pin)
 * uint16_t -> pgm_read_word()
 * uint32_t -> pgm_read_dword()
 * ********************************************************************/
const struct motor_config_t motor_config[] PROGMEM =
{ //{motorType, limit, defaultSpeedForward, defaultSpeedBackwards, motorIdx, dir_pin/rpwm_pin, pul_pin/lpwm_pin, en_pin, calib_pin, relay_pin, orientation}
    {STEPPER_MOTOR, 6700,   3*P_BASE,   3*P_BASE,  MOTOR_DIR_RIGHT,     2,  3,  4,  5,  6,  0},
    {STEPPER_MOTOR, 6700,   3*P_BASE,   3*P_BASE,  MOTOR_DIR_LEFT,      2,  3,  4,  5,  6,  0},
    {PWM_MOTOR,     0,      200,        200,       MOTOR_TRACTION_RIGHT,5,  2,  17, 0,  3,  0},
    {PWM_MOTOR,     0,      200,        200,       MOTOR_TRACTION_LEFT, 9,  10, A3, 0,  A2, 0},
    {STEPPER_PID,   6700,   3*P_BASE,   3*P_BASE,  MOTOR_X,             4,  3,  5,  0,  A1, 0}};



/****************************** Structures *************************************/

// store dynamic parameters in RAM, bytes alligned and optimized
struct motor_control_t
{
    volatile uint32_t pos;      // Current motor position
    int vel;                    // motor speed ("ticks period")
    int counterHandler;         // handler counter for speed control
    uint8_t dir : 1;            // current motor direction, 1 -> increments ticks, 0 -> decrements ticks
    uint8_t motorRunFlag : 1;   // flag to indicate the current motor operation status
    uint8_t cmdAnterior : 2;    // previous given command to the motor controller
    volatile uint8_t toggle : 1;// flag to control the controller pulses
    uint8_t calibFlag : 1;      // flag to indicate if the motor is calibrated, can be also used to control the motor without pulses
};

/****************************** Function Prototypes *************************************/

void printValuesMotor(MOTOR_ENUM motor);
void motors_init(void);
void motor_control(MOTOR_ENUM motor, MOTOR_CONTROL cmd, uint32_t period, MOTOR_TYPES motorType);
void disable_motor(MOTOR_ENUM motor);
void enable_motor(MOTOR_ENUM motor);
void calibrateMotor(MOTOR_ENUM motor);
void goto_pos(MOTOR_ENUM motor, uint32_t targetPos, uint32_t period);

#endif