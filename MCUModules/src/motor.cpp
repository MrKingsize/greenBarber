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
void printValuesMotor(MOTOR_ENUM motor)
{
	if (motor < MOTOR_LAST)
	{
		char buffer[50];
		sprintf (buffer,"MOTOR_ID: %d\tpos = %d\tdir = %d\tmotor_run = %d\n",(int)motor, (int)motorCtrl[motor].pos,motorCtrl[motor].dir,motorCtrl[motor].motorRunFlag);
		Serial.print(buffer);
	}
}

/*******************************************************************************
 * @brief Motor activation function
 *******************************************************************************/
void enable_motor(MOTOR_ENUM motor)
{
	if (motor < MOTOR_LAST)
	{
		digitalWrite(pgm_read_byte(&motor_config[motor].en_pin), LOW);
	} 
}

/*******************************************************************************
 * @brief Motor deactivation function
 *******************************************************************************/
void disable_motor(MOTOR_ENUM motor)
{
	if (motor < MOTOR_LAST)
	{
		digitalWrite(pgm_read_byte(&motor_config[motor].en_pin), HIGH);
	}  
}

/*******************************************************************************
 * @brief go to position control function without PID
 *******************************************************************************/
void goto_pos(MOTOR_ENUM motor, uint32_t targetPos, uint32_t period)
{
	if (motor < MOTOR_LAST)
	{
		if (targetPos > motorCtrl[motor].pos)
		{
			while(motorCtrl[motor].pos < targetPos && motorCtrl[motor].pos < pgm_read_dword(&motor_config[motor].limit))
			{
				motor_control(motor,MOTOR_PLUS_CMD,period,STEPPER_MOTOR);
			}
		}
		else
		{
			while(motorCtrl[motor].pos > targetPos && motorCtrl[motor].pos > 0)
			{
				motor_control(motor,MOTOR_MINUS_CMD,period,STEPPER_MOTOR);
			}
		}
		motorCtrl[motor].motorRunFlag = 0;
		//motor_control(motor,MOTOR_STOP_CMD,period);
	}
}

/******************************************************************************
 * @brief calibrates motor
 ******************************************************************************/
void calibrateMotor(MOTOR_ENUM motor)
{
  
	while(digitalRead(pgm_read_byte(&motor_config[motor].calib_pin)) == 1){
		motor_control(motor,MOTOR_MINUS_CMD,pgm_read_word(&motor_config[motor].defaultSpeedBackwards),STEPPER_MOTOR);
		delay(1);
	}
	motor_control(motor,MOTOR_STOP_CMD,pgm_read_word(&motor_config[motor].defaultSpeedBackwards),STEPPER_MOTOR);
	//printf("Zero marked!\n");
	motorCtrl[motor].calibFlag = 1;
	motorCtrl[motor].pos = 0;
	goto_pos(motor, 2000, pgm_read_word(&motor_config[motor].defaultSpeedForward));
	motor_control(motor,MOTOR_STOP_CMD,pgm_read_word(&motor_config[motor].defaultSpeedBackwards),STEPPER_MOTOR);
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
		if ((period % P_BASE) != 0)
		{
			return;
		}
		
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
				motorCtrl[motor].vel = period;
				break;
			case MOTOR_MINUS_CMD:
				motorCtrl[motor].motorRunFlag = 1;
				motorCtrl[motor].dir = 0;
				enable_motor(motor);
				motorCtrl[motor].vel = period;   
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
	uint8_t motorIdx;
	for (uint8_t motorIdxModule = 0; motorIdxModule < motorsCount; motorIdxModule++)
	{
		motorIdx = motorsList[motorIdxModule];
		if (motorCtrl[motorIdx].motorRunFlag == 1)
		{
			if (motorCtrl[motorIdx].counterHandler == motorCtrl[motorIdx].vel)
			{
				
				if (motorCtrl[motorIdx].dir == pgm_read_byte(&motor_config[motorIdx].orientation))
					digitalWrite(pgm_read_byte(&motor_config[motorIdx].dir_or_rpwm_pin), LOW);
				else
					digitalWrite(pgm_read_byte(&motor_config[motorIdx].dir_or_rpwm_pin), HIGH);
				
				
				if (motorCtrl[motorIdx].toggle == 1)
				{
					if (motorCtrl[motorIdx].calibFlag == 0)
					{
						motorCtrl[motorIdx].toggle = 0;
						digitalWrite(pgm_read_byte(&motor_config[motorIdx].pul_or_lpwm_pin), HIGH);
						if (motorCtrl[motorIdx].dir == 1)
						{
						motorCtrl[motorIdx].pos++;
						}
						else
						{
						motorCtrl[motorIdx].pos--;
						}
					}
					else
					{
						if (motorCtrl[motorIdx].dir == 1)
						{
							if ((motorCtrl[motorIdx].pos < pgm_read_dword(&motor_config[motorIdx].limit)) || (pgm_read_dword(&motor_config[motorIdx].limit) == 0))
							{
								motorCtrl[motorIdx].pos++;
								motorCtrl[motorIdx].toggle = 0;
								digitalWrite(pgm_read_byte(&motor_config[motorIdx].pul_or_lpwm_pin), HIGH);
							}
						}
						else
						{
							if (motorCtrl[motorIdx].pos != 0)
							{
								motorCtrl[motorIdx].pos--;
								motorCtrl[motorIdx].toggle = 0;
								digitalWrite(pgm_read_byte(&motor_config[motorIdx].pul_or_lpwm_pin),HIGH);
							}
						}
					}
				}
				else
				{
					motorCtrl[motorIdx].toggle = 1;
					digitalWrite(pgm_read_byte(&motor_config[motorIdx].pul_or_lpwm_pin),LOW);
				}
				motorCtrl[motorIdx].counterHandler = 0;
			}

		motorCtrl[motorIdx].counterHandler += P_BASE;
		}
	}
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