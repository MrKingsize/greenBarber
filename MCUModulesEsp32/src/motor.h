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

/*
Driver TB6600 switch configuration table

For a compromisse with high torque and smooth operation we choose: 800 pulses/rev
Microstep	| Pulse/ver |	S1	|	S2	|	S3
-----------------------------------------------
	NC		|	NC		|	ON	|	ON	|	ON
	1		|	200		|	ON	|	ON	|	OFF
	2/A		|	400		|	ON	|	OFF	|	ON
	2/B		|	400		|	OFF	|	ON	|	ON
	4		|	800		|	ON	|	OFF	|	OFF
	8		|	1600	|	OFF	|	ON	|	OFF
	16		|	3200	|	OFF	|	OFF	|	ON
	32		|	6400	|	OFF	|	OFF	|	OFF
				
// Based on motor rated at 1.68A, choosing the safest setting: 1.5A RMS. for 17HS19 (nema 17)
Crrent(A)	| PKCurrent |	S4	|	S5	|	S6
-----------------------------------------------
	0,5		|	0,7		|	ON	|	ON	|	ON
	1		|	1,2		|	ON	|	OFF	|	ON
	1,5		|	1,7		|	ON	|	ON	|	OFF
	2		|	2,2		|	ON	|	OFF	|	OFF
	2,5		|	2,7		|	OFF	|	ON	|	ON
	2,8		|	2,9		|	OFF	|	OFF	|	ON
	3		|	3,2		|	OFF	|	ON	|	OFF
	3,5		|	4		|	OFF	|	OFF	|	OFF
*/

// debug
#define MOTORDEBUG
#define DEBUGCOUNTER (1000000 / P_BASE)
#define DEBUGCOUNTER_PID (100000 / P_BASE)

// rotation speed - choose only multiples of P_BASE.
#define P_BASE             40 // 40 us
//#define MOTOR_LAST         (sizeof(motor_config) / sizeof(motor_config[0]))
#define MAX_MOTORS_PER_MODULE   6

#define INTERNAL_WHEEL_FACTOR 0.8928
#define EXTERNAL_WHEEL_FACTOR 1.0768


#define PID_UPDATE_TICKS     25          // 25 * 40us = 1ms (not used)
#define POS_DEADBAND         5

#define PID_SHIFT            8
#define PID_INT_MAX          (200000L << PID_SHIFT) // give I room, you were clipping too early

// “Speed units” are arbitrary internal units (not Hz). Tune to taste.
#define PID_OUT_MAX_SPEED    12000
#define PID_OUT_MIN_SPEED    200

// Feed‑forward mapping (|error| -> base speed)
#define FF_ERR_MAX           4000        // above this error, use max FF speed
#define FF_SPEED_MIN         200
#define FF_SPEED_MAX         10000

// Period limits, in ISR ticks (40us units)
#define MIN_PERIOD_40US      100           // fastest (not used)
#define MAX_PERIOD_40US      2000        // slowest



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
	MOTOR_TURBINE_1,
	MOTOR_TURBINE_2,
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


const uint8_t MCU_MAIN_MOTORS[] = {
	MOTOR_TRACTION_RIGHT,
};

const uint8_t HW_DIR_RIGHT_MOTORS[] = {
	MOTOR_DIR_RIGHT
};

const uint8_t HW_DIR_LEFT_MOTORS[] = {
	MOTOR_DIR_LEFT
};

const uint8_t HW_X_TRACTION_LEFT_MOTORS[] = {
	MOTOR_TRACTION_LEFT,
	MOTOR_X
};

const uint8_t HW_Y_MOTORS[] = {
	MOTOR_Y
};

const uint8_t HW_Z_MOTORS[] = {
	MOTOR_Z
};

const uint8_t HW_CUT_MOTORS[] = {
	MOTOR_ROTATE,
	MOTOR_CUT
};

const uint8_t HW_TURBINES_MOTORS[] = {
	MOTOR_TURBINE_1,
	MOTOR_TURBINE_2,
	MOTOR_GATE_1,
	MOTOR_GATE_2
};

// Fixed motor parameters 
struct motor_config_t
{
	const MOTOR_TYPES motorType;                        // Motor type
	const uint32_t limit;                               // End limit of the motor, 0 = infinite
	const uint8_t defaultSpeedForward;                 // STEPPER: máx PID speed or PWM: default speed which the motor moves forwards
	const uint8_t defaultSpeedBackwards;               // default speed which the motor moves backwards
	const MOTOR_ENUM motorIdx;                          // Modor Idx MOTOR_ENUM
	const uint8_t dir_or_rpwm_pin;                      // Driver direction pin or Right PWM pin
	const uint8_t pul_or_lpwm_pin;                      // Driver Pulse pin or Left PWM pin
	const uint8_t en_pin;                               // Driver Enable pin
	const uint8_t calib_pin;                            // End limit pin, 0 = no pin
	const uint8_t relay_pin;                            // Power Supply relay pin
	const uint8_t orientation;                          // dir value which pulses increment position
	const uint16_t PID_P;                                // PID P value
	const uint16_t PID_I;                                // PID I value
	const uint16_t PID_D;                                // PID D value
	const uint8_t holdFlag;                            // Hold flag, 0 = no hold, 1 = hold enabled
};


/**********************************************************************
 * store fixed parameters in flash ()
 * Note: parameter motorIdx has to allways be by order and continuously according to MOTOR_ENUM
 * GPIO5:
 * Strapping pin → at boot it decides SDIO slave timing together with MTDO.
 * Has an internal pull-up by default.
 * If you pull it low during reset, it may affect boot.
 * After boot, it’s a normal digital I/O (safe for GPIO, PWM, etc.).
 * ********************************************************************/
const struct motor_config_t motor_config[] =
{//{motorType,limit,defaultSpeedForward,defaultSpeedBackwards,motorIdx,dir_pin/rpwm_pin,pul_pin/lpwm_pin,en_pin,calib_pin,relay_pin,orientation,PID_P,PID_I,PID_D,Holdflag}
	{STEPPER_MOTOR,	4000,	15,		125,					MOTOR_DIR_RIGHT,		15,			5,			4,		19,		18,			0,		100,	1,	0,		1},
	{STEPPER_MOTOR,	2000,	40,		125,					MOTOR_DIR_LEFT,			15,			5,			4,		19,		18,			0,		100,	1,	0,		1},
	{PWM_MOTOR,		0,		200,		200,				MOTOR_TRACTION_RIGHT,	15,			5,			4,		19,		18,			0,		0,		0,	0,		1},
	{PWM_MOTOR,		0,		200,		200,				MOTOR_TRACTION_LEFT,	15,			13,			4,		19,		18,			0,		0,		0,	0,		1},
	{STEPPER_PID,	6700,	3,			3,					MOTOR_X,				4,			3,			5,		0,		0,			0,		0,		0,	0,		1},};



/****************************** Structures *************************************/

// store dynamic parameters in RAM, bytes alligned and optimized
struct motor_control_t
{
	volatile uint32_t pos;      // Current motor position
	uint32_t targetPos;         // Target position
	int period;                    // motor speed ("ticks period")
	int counterHandler;         // handler counter for speed control
	uint8_t dir : 1;            // current motor direction, 1 -> increments ticks, 0 -> decrements ticks
	uint8_t motorRunFlag : 1;   // flag to indicate the current motor operation status
	uint8_t cmdAnterior : 2;    // previous given command to the motor controller
	volatile uint8_t toggle : 1;// flag to control the controller pulses
	uint8_t calibFlag : 1;      // flag to indicate if the motor is calibrated, can be also used to control the motor without pulses
	int32_t  pid_integral;      // PID integral value
	int32_t  pid_prev_err;      // PID previous error value
	uint16_t pid_counter;       // PID counter for the handler
};

/****************************** Function Prototypes *************************************/

void printValuesMotor(MOTOR_ENUM motor);
void motors_init(void);
void motor_control(MOTOR_ENUM motor, MOTOR_CONTROL cmd, uint32_t period, MOTOR_TYPES motorType);
void disable_motor(uint8_t motor);
void enable_motor(uint8_t motor);
void calibrateMotor(MOTOR_ENUM motor);
void goto_pos(MOTOR_ENUM motor, uint32_t targetPos, uint32_t modeSpeed);
uint32_t angleToTick(int16_t angle, uint8_t motorID);
void readMotorState(uint8_t motorID, uint8_t *runFlagCheck);
uint8_t getCalibratedFlag(MOTOR_ENUM motor);
uint32_t motor_get_pos(uint8_t motor);

#endif