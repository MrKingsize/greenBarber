/** @file motor.cpp
 * 
 * @brief File to control the motors. 
 *
 */ 

/****************************** Includes *************************************/
#include <Arduino.h>
#include <TimerOne.h>
//#include <MsTimer2.h>

#include "motor.h"
#include "module_map.h"
#include "common.h"

/*************************** Global Variables ********************************/

motor_control_t motorCtrl[MOTOR_LAST] = {};
uint8_t motorsList[MAX_MOTORS_PER_MODULE] = {};
uint8_t motorsCount = 0;
uint8_t motorsListPWM[MAX_MOTORS_PER_MODULE] = {};
uint8_t motorsCountPWM = 0;

uint32_t countPrintMotor = 0; // Debug variables

/****************************** Functions *************************************/


/*******************************************************************************
 * @brief print of the motor control values
 *******************************************************************************/
void printValuesMotor(uint8_t motor)
{
	if (motor < MOTOR_LAST)
	{
		DEBUG_PRINTF("MOTOR_ID: %d\tpos = %d\tdir = %d\tmotor_run = %d\n",(int)motor, (int)motorCtrl[motor].pos, motorCtrl[motor].dir, motorCtrl[motor].motorRunFlag);
	}
}

void printValuesPid(uint8_t motor)
{
	if (motor < MOTOR_LAST)
	{
		DEBUG_PRINTF("period: %d\tpid_prev_err = %d\tpid_integral = %d\n",(int)motorCtrl[motor].period, (int)motorCtrl[motor].pid_prev_err, motorCtrl[motor].pid_integral);
	}
}

/*******************************************************************************
 * @brief Motor activation function
 *******************************************************************************/
void enable_motor(uint8_t motor)
{
	if (motor < MOTOR_LAST)
	{
		digitalWrite(pgm_read_byte(&motor_config[motor].en_pin), LOW);
		//TRACE_PRINTF("Motor %d enabled, pin %d, value %d\n", motor, pgm_read_byte(&motor_config[motor].en_pin), LOW);
	} 
}

/*******************************************************************************
 * @brief Motor deactivation function
 *******************************************************************************/
void disable_motor(uint8_t motor)
{
	if (motor < MOTOR_LAST)
	{
		digitalWrite(pgm_read_byte(&motor_config[motor].en_pin), HIGH);
	}  
}

/*******************************************************************************
 * @brief Clamps a value between a minimum and maximum
 * @param v Value to clamp
 * @param lo Minimum value
 * @param hi Maximum value
 * @return Clamped value
 *******************************************************************************/
static inline int32_t clamp_i32(int32_t v, int32_t lo, int32_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/*******************************************************************************
 * @brief Maps a value from one range to another
 * @param x Value to map
 * @param in_min Minimum of the input range
 * @param in_max Maximum of the input range
 * @param out_min Minimum of the output range
 * @param out_max Maximum of the output range
 * @return Mapped value
 *******************************************************************************/
static inline int32_t map_linear_i32(int32_t x, int32_t in_min, int32_t in_max,
                                     int32_t out_min, int32_t out_max)
{
    if (x <= in_min) return out_min;
    if (x >= in_max) return out_max;
    return out_min + (int64_t)(out_max - out_min) * (x - in_min) / (in_max - in_min);
}

/*******************************************************************************
 * @brief Converts error to feed-forward speed
 * @param abs_err Absolute error in [0, FF_ERR_MAX]
 * @return Speed in [FF_SPEED_MIN, FF_SPEED_MAX]
 *******************************************************************************/
static inline int32_t ff_error_to_speed(int32_t abs_err)
{
    return map_linear_i32(abs_err, 0, FF_ERR_MAX, FF_SPEED_MIN, FF_SPEED_MAX);
}

/*******************************************************************************
 * @brief Converts speed to period in 40us units
 * @param speed Speed in [0, PID_OUT_MAX_SPEED]
 * @return Period in 40us units
 *******************************************************************************/
static inline uint16_t speed_to_period_40us(int32_t speed, uint8_t motorIdx)
{
    speed = clamp_i32(speed, PID_OUT_MIN_SPEED, PID_OUT_MAX_SPEED);

    // Linear map (fast speed -> small period)
    uint32_t num   = (uint32_t)(PID_OUT_MAX_SPEED - speed);
    uint32_t denom = (PID_OUT_MAX_SPEED - PID_OUT_MIN_SPEED);

	uint16_t minPeriod = pgm_read_byte(&motor_config[motorIdx].defaultSpeedForward);
    uint32_t period = minPeriod +
        ((uint32_t)(MAX_PERIOD_40US - minPeriod) * num) / denom;

    return (uint16_t)period;
}

/*******************************************************************************
 * @brief PID update function
 * @param motorIdx Index of the motor to update
 *******************************************************************************/
static void pid_update(uint8_t motorIdx)
{
    // Gains (raw -> fixed point)
    uint16_t kp_raw = pgm_read_word(&motor_config[motorIdx].PID_P);
    uint16_t ki_raw = pgm_read_word(&motor_config[motorIdx].PID_I);
    uint16_t kd_raw = pgm_read_word(&motor_config[motorIdx].PID_D);

    int32_t Kp = ((int32_t)kp_raw) << PID_SHIFT;
    int32_t Ki = ((int32_t)ki_raw) << PID_SHIFT;
    int32_t Kd = ((int32_t)kd_raw) << PID_SHIFT;

    int32_t error = (int32_t)motorCtrl[motorIdx].targetPos - (int32_t)motorCtrl[motorIdx].pos;
    int32_t abs_err = (error >= 0) ? error : -error;

    // Deadband stop
    if (abs_err <= POS_DEADBAND) {
        motorCtrl[motorIdx].motorRunFlag = 0;
        motorCtrl[motorIdx].period = MAX_PERIOD_40US;
        motorCtrl[motorIdx].pid_integral = 0;
        motorCtrl[motorIdx].pid_prev_err = error;
		uint8_t holdFlag = pgm_read_byte(&motor_config[motorIdx].holdFlag);
		if (holdFlag == 0)
			disable_motor(motorIdx);
		DEBUG_PRINTF("Motor %u stopped, tick error: %ld\n", (unsigned)motorIdx, (long)error);
        return;
    }

    // Direction
    motorCtrl[motorIdx].dir = (error > 0) ? 1 : 0;

    // Feed-forward base speed
    int32_t ff_speed = ff_error_to_speed(abs_err);

    // Derivative
    int32_t deriv = error - motorCtrl[motorIdx].pid_prev_err;
    motorCtrl[motorIdx].pid_prev_err = error;

    // Predict PID output *before* integrating to decide about windup
    int32_t pid_out_fp_preview =
        (Kp * error) +
        (Ki * motorCtrl[motorIdx].pid_integral) +
        (Kd * deriv);
    int32_t pid_out_preview = pid_out_fp_preview >> PID_SHIFT;

    int32_t speed_preview = ff_speed + ((pid_out_preview >= 0) ? pid_out_preview : -pid_out_preview);
    bool will_saturate = (speed_preview >= PID_OUT_MAX_SPEED);

    // Integrate ONLY if not saturated (simple anti-windup)
    if (!will_saturate) {
        motorCtrl[motorIdx].pid_integral += error;
        motorCtrl[motorIdx].pid_integral =
            clamp_i32(motorCtrl[motorIdx].pid_integral, -PID_INT_MAX, PID_INT_MAX);
    }

    // Now compute final PID
    int32_t pid_out_fp =
        (Kp * error) +
        (Ki * motorCtrl[motorIdx].pid_integral) +
        (Kd * deriv);
    int32_t pid_out = pid_out_fp >> PID_SHIFT;

    // Combine FF + PID (pid_out is signed; speed magnitude is positive)
    int32_t speed_cmd = ff_speed + ((pid_out >= 0) ? pid_out : -pid_out);

    // To period
    uint16_t period = speed_to_period_40us(speed_cmd, motorIdx);
    motorCtrl[motorIdx].period = period;
}

/*******************************************************************************
 * @brief Initializes the motors
 *******************************************************************************/
void goto_pos(MOTOR_ENUM motor, uint32_t targetPos)
{
	enable_motor(motor);
	motorCtrl[motor].targetPos = targetPos;
	motorCtrl[motor].motorRunFlag = 1;
}

/******************************************************************************
 * @brief calibrates motor
 ******************************************************************************/
void calibrateMotor(MOTOR_ENUM motor)
{
	TRACE_PRINTF("Calibrating Motor %d\n", motor);
	uint8_t calibPin = pgm_read_byte(&motor_config[motor].calib_pin);
	while(digitalRead(calibPin) == 1){
		motor_control(motor, MOTOR_MINUS_CMD, pgm_read_byte(&motor_config[motor].defaultSpeedBackwards), STEPPER_MOTOR);
		delay(1);
	}
	//motor_control(motor, MOTOR_STOP_CMD, pgm_read_byte(&motor_config[motor].defaultSpeedBackwards), STEPPER_MOTOR);
	TRACE_PRINTF("Zero reseted!\n");
	motorCtrl[motor].calibFlag = 1;
	motorCtrl[motor].pos = 0;
	//goto_pos(motor, 2000);
	//motor_control(motor, MOTOR_STOP_CMD, pgm_read_byte(&motor_config[motor].defaultSpeedBackwards), STEPPER_MOTOR);
}

/******************************************************************************
 * @brief motor control function
 ******************************************************************************/
void motor_control(MOTOR_ENUM motor, MOTOR_CONTROL cmd, uint32_t period, MOTOR_TYPES motorType)
{
	

	if (motor >= MOTOR_LAST)
		return;

	

	if (motorType != PWM_MOTOR)
	{
			
		if (motorCtrl[motor].cmdAnterior == cmd)
		return;

		switch(cmd)
		{
			case MOTOR_STOP_CMD:
				motorCtrl[motor].motorRunFlag = 0;
				disable_motor(motor);   
				break;
			case MOTOR_PLUS_CMD:
				motorCtrl[motor].motorRunFlag = 1;
				motorCtrl[motor].dir = 1;
				enable_motor(motor);
				motorCtrl[motor].period = period;
				break;
			case MOTOR_MINUS_CMD:
				motorCtrl[motor].motorRunFlag = 1;
				motorCtrl[motor].dir = 0;
				enable_motor(motor);
				motorCtrl[motor].period = period;   
				break;
		}
		motorCtrl[motor].cmdAnterior = cmd;
	}
	else // PWM Motor
	{
		uint8_t motorIdx = motor;
		
		switch(cmd)
		{
			case MOTOR_STOP_CMD:
				analogWrite(pgm_read_byte(&motor_config[motorIdx].dir_or_rpwm_pin), 0);
  				analogWrite(pgm_read_byte(&motor_config[motorIdx].pul_or_lpwm_pin), 0);
				break;
			case MOTOR_PLUS_CMD:
				analogWrite(pgm_read_byte(&motor_config[motorIdx].dir_or_rpwm_pin), period);
  				analogWrite(pgm_read_byte(&motor_config[motorIdx].pul_or_lpwm_pin), 0);
				digitalWrite(pgm_read_byte(&motor_config[motorIdx].en_pin), HIGH);
				break;
			case MOTOR_MINUS_CMD:
				analogWrite(pgm_read_byte(&motor_config[motorIdx].dir_or_rpwm_pin), 0);
  				analogWrite(pgm_read_byte(&motor_config[motorIdx].pul_or_lpwm_pin), period);
				digitalWrite(pgm_read_byte(&motor_config[motorIdx].en_pin), HIGH);
				break;
		}
	}
}

/******************************************************************************
 * @brief Motor control handler
 ******************************************************************************/
static void motor_handler()
{
#ifdef MOTORDEBUG
	static uint32_t handlerCount = 0;
    static uint32_t nextPidPrint    = DEBUGCOUNTER_PID;  // 100 ms
    static uint32_t nextMotorPrint  = DEBUGCOUNTER;      // 1000 ms

#endif
	uint8_t motorIdx;
	for (uint8_t motorIdxModule = 0; motorIdxModule < motorsCount; motorIdxModule++)
	{

		motorIdx = motorsList[motorIdxModule];

		if (motorCtrl[motorIdx].motorRunFlag == 1)
		{
			// --- PID update every 1ms ---
			if ((++motorCtrl[motorIdx].pid_counter >= PID_UPDATE_TICKS) && (motorCtrl[motorIdx].calibFlag == 1)) {
				motorCtrl[motorIdx].pid_counter = 0;
				pid_update(motorIdx);
			}

			// --- Stepping logic each ISR tick ---
			if (++motorCtrl[motorIdx].counterHandler >= motorCtrl[motorIdx].period)
			{
				motorCtrl[motorIdx].counterHandler = 0;

				// Set direction pin
				if (motorCtrl[motorIdx].dir == pgm_read_byte(&motor_config[motorIdx].orientation))
					digitalWrite(pgm_read_byte(&motor_config[motorIdx].dir_or_rpwm_pin), LOW);
				else
					digitalWrite(pgm_read_byte(&motor_config[motorIdx].dir_or_rpwm_pin), HIGH);
				
				// process motor pulses
				if (motorCtrl[motorIdx].toggle) {
					// Rising edge: actually step (if allowed)
					motorCtrl[motorIdx].toggle = 0;
					digitalWrite(pgm_read_byte(&motor_config[motorIdx].pul_or_lpwm_pin), HIGH);

					if (motorCtrl[motorIdx].calibFlag == 0) {
						// Not calibrated: free run
						motorCtrl[motorIdx].pos += (motorCtrl[motorIdx].dir ? 1 : -1);
					} else {
						// Calibrated: enforce limits
						uint32_t limit = pgm_read_dword(&motor_config[motorIdx].limit);

						if (motorCtrl[motorIdx].dir) { // moving +
							if ((limit == 0) || (motorCtrl[motorIdx].pos < limit)) {
								motorCtrl[motorIdx].pos++;
							} else {
								// Hit upper limit -> stop
								motorCtrl[motorIdx].motorRunFlag = 0;
							}
						} else { // moving -
							if (motorCtrl[motorIdx].pos > 0) {
								motorCtrl[motorIdx].pos--;
							} else {
								// Hit lower (0) limit -> stop
								motorCtrl[motorIdx].motorRunFlag = 0;
							}
						}
					}

				}
				else
				{
					motorCtrl[motorIdx].toggle = 1;
					digitalWrite(pgm_read_byte(&motor_config[motorIdx].pul_or_lpwm_pin),LOW);
				}
				
			}
		}
	}


#ifdef MOTORDEBUG

		// ---- Now do the timing logic ONCE per ISR, not per motor ----
    handlerCount++;

    // Print every 100 ms
    if (handlerCount >= nextPidPrint) {
        for (uint8_t motorIdxModule = 0; motorIdxModule < motorsCount; motorIdxModule++) {
            uint8_t motorIdx = motorsList[motorIdxModule];
            if (motorCtrl[motorIdx].motorRunFlag) {
                printValuesPid(motorIdx);
            }
        }
        nextPidPrint += DEBUGCOUNTER_PID;
    }

    // Print every 1000 ms
    if (handlerCount >= nextMotorPrint) {
        for (uint8_t motorIdxModule = 0; motorIdxModule < motorsCount; motorIdxModule++) {
            uint8_t motorIdx = motorsList[motorIdxModule];
            if (motorCtrl[motorIdx].motorRunFlag) {
                printValuesMotor(motorIdx);
            }
        }
        nextMotorPrint += DEBUGCOUNTER;
    }

	// Optional: prevent overflow drift
    if (handlerCount >= DEBUGCOUNTER * 100UL) { // e.g. every 100s
        handlerCount   = 0;
        nextPidPrint   = DEBUGCOUNTER_PID;
        nextMotorPrint = DEBUGCOUNTER;
    }
		
#endif
}

/******************************************************************************
 * @brief Init motors
 ******************************************************************************/
void motors_init(void){
	TRACE_PRINTF("Entering Function\n");

	uint8_t myID = get_my_module_id();

	const module_cmd_t *mod = &moduleMap[myID];
	const uint8_t *motorListPtr = (const uint8_t *)pgm_read_ptr(&mod->motorsList);
	uint8_t motorCount = pgm_read_byte(&mod->motorsCount);

	for (uint8_t j = 0; j < motorCount; j++) 
	{
		uint8_t motorIdx = pgm_read_byte(&motorListPtr[j]);
		DEBUG_PRINTF("Configuring motor %d\n", motorIdx);
		// Inicializa só estes motores
		if (motorIdx < sizeof(motor_config) / sizeof(motor_config[0])) {
			// Safe to access motor_config[motorIdx]
			pinMode(pgm_read_byte(&motor_config[motorIdx].en_pin), OUTPUT);
			pinMode(pgm_read_byte(&motor_config[motorIdx].dir_or_rpwm_pin), OUTPUT);
			pinMode(pgm_read_byte(&motor_config[motorIdx].pul_or_lpwm_pin), OUTPUT);
			digitalWrite(pgm_read_byte(&motor_config[motorIdx].relay_pin),HIGH); // TODO set pin pullup!
			pinMode(pgm_read_byte(&motor_config[motorIdx].relay_pin), OUTPUT);
			DEBUG_PRINTF("en_pin %d OUTPUT\n",	pgm_read_byte(&motor_config[motorIdx].en_pin));
			DEBUG_PRINTF("dir_or_rpwm_pin %d OUTPUT\n",	pgm_read_byte(&motor_config[motorIdx].dir_or_rpwm_pin));
			DEBUG_PRINTF("pul_or_lpwm_pin %d OUTPUT\n",	pgm_read_byte(&motor_config[motorIdx].pul_or_lpwm_pin));
			DEBUG_PRINTF("relay_pin %d OUTPUT\n",	pgm_read_byte(&motor_config[motorIdx].relay_pin));
			if (pgm_read_byte(&motor_config[motorIdx].motorType) != PWM_MOTOR)
			{
				pinMode(pgm_read_byte(&motor_config[motorIdx].calib_pin), INPUT_PULLUP);
				DEBUG_PRINTF("calib_pin %d INPUT_PULLUP\n",	pgm_read_byte(&motor_config[motorIdx].calib_pin));
			}
		} else {
			DEBUG_PRINTF("motorIdx %d pinout not mapped\n", motorIdx);
		}
	}

	if (myID != MOD_SEL_MCU_MAIN)
	{
		Timer1.initialize(P_BASE);
		Timer1.attachInterrupt(motor_handler);
	}

	// Fill motorsList
	get_motors_list(motorsList,	&motorsCount, motorsListPWM, &motorsCountPWM);
	TRACE_PRINTF("Exiting Function\n");
	DEBUG_PRINTF("motorsCount=%d, motorsCountPWM=%d\n",motorsCount,motorsCountPWM);
	for (uint8_t i = 0; i < motorsCount; i++)
	{
		DEBUG_PRINTF("motorsList[%d]=%d\n",i, motorsList[i]);
	}
	for (uint8_t i = 0; i < motorsCountPWM; i++)
	{
		DEBUG_PRINTF("motorsListPWM[%d]=%d\n",i, motorsListPWM[i]);
	}
	
	// Orderly turn on Power supplies, activation delay is calculated from a formula based on the motor Index
	uint16_t delayMsAcc = 0;
	for (uint8_t j = 0; j < motorCount; j++) 
	{

		uint8_t motorIdx = pgm_read_byte(&motorListPtr[j]);
		uint16_t delayMs = (motorIdx + 1)*1000 - delayMsAcc;
		delayMsAcc += delayMs;
		DEBUG_PRINTF("Powering Up motor %d... delay = %d\n", motorIdx, delayMs);
		delay(delayMs);
		digitalWrite(pgm_read_byte(&motor_config[motorIdx].relay_pin),LOW);
		DEBUG_PRINTF("Motor %d Power Up Success!\n", motorIdx);
	}

	if (myID == MOD_SEL_MCU_MAIN)
	{
		// after all modules power up. Activate main power Relay
		uint16_t delayMs = (MOTOR_LAST + 1)*1000 - delayMsAcc;
		delayMsAcc += delayMs;
		DEBUG_PRINTF("Waiting for all modules power up... delay = %d\n", delayMs);
		delay(delayMs);
		pinMode(13, OUTPUT);
		digitalWrite(13,HIGH);
		DEBUG_PRINTF("Main power relay activated\nSystem online!\n");
	}
}

/******************************************************************************
 * @brief Converts the motor target angle to the stepper motor tick value
 * input -> output
 * limit/2 -> 0
 * 0 -> -45
 * limit -> 45
 ******************************************************************************/
uint32_t angleToTick(int16_t angle, uint8_t motorID)
{
	uint32_t limitDir = pgm_read_dword(&motor_config[motorID].limit);
	// Calculate the motor tick  based on the angle and limit
	double motorTick = ((double)(angle + 45) / 90.0) * limitDir;
	if (motorTick < 0)
	    motorTick = 0;
	return (uint32_t)motorTick;
}

/******************************************************************************
 * @brief Return motor run flag
 ******************************************************************************/
void readMotorState(uint8_t motorID, uint8_t *runFlagCheck)
{
	*runFlagCheck = motorCtrl[motorID].motorRunFlag;
}