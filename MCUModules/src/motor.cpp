/** @file motor.cpp
 * 
 * @brief File to control the motors. 
 *
 */ 

/****************************** Includes *************************************/
#include <Arduino.h>
#include <TimerOne.h>

#include "motor.h"
#include "module_map.h"

/*************************** Global Variables ********************************/

motor_control_t motorCtrl[MOTOR_LAST] = {};
uint8_t motorsList[MAX_MOTORS_PER_MODULE] = {};
uint8_t motorsCount;

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
				motor_control(motor,MOTOR_PLUS_CMD,period);
			}
		}
		else
		{
			while(motorCtrl[motor].pos > targetPos && motorCtrl[motor].pos > 0)
			{
				motor_control(motor,MOTOR_MINUS_CMD,period);
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
		motor_control(motor,MOTOR_MINUS_CMD,pgm_read_word(&motor_config[motor].defaultSpeedBackwards));
		delay(1);
	}
	motor_control(motor,MOTOR_STOP_CMD,pgm_read_word(&motor_config[motor].defaultSpeedBackwards));
	//printf("Zero marked!\n");
	motorCtrl[motor].calibFlag = 1;
	motorCtrl[motor].pos = 0;
	goto_pos(motor, 2000, pgm_read_word(&motor_config[motor].defaultSpeedForward));
	motor_control(motor,MOTOR_STOP_CMD,pgm_read_word(&motor_config[motor].defaultSpeedBackwards));
}

/******************************************************************************
 * @brief motor control function
 ******************************************************************************/
void motor_control(MOTOR_ENUM motor, MOTOR_CONTROL cmd, uint32_t period)
{
	if ((period % P_BASE) != 0)
	{
		return;
	}

	if (motor >= MOTOR_LAST)
		return;

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
					digitalWrite(pgm_read_byte(&motor_config[motorIdx].dir_pin), LOW);
				else
					digitalWrite(pgm_read_byte(&motor_config[motorIdx].dir_pin), HIGH);
				
				
				if (motorCtrl[motorIdx].toggle == 1)
				{
					if (motorCtrl[motorIdx].calibFlag == 0)
					{
						motorCtrl[motorIdx].toggle = 0;
						digitalWrite(pgm_read_byte(&motor_config[motorIdx].pul_pin), HIGH);
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
								digitalWrite(pgm_read_byte(&motor_config[motorIdx].pul_pin), HIGH);
							}
						}
						else
						{
							if (motorCtrl[motorIdx].pos != 0)
							{
								motorCtrl[motorIdx].pos--;
								motorCtrl[motorIdx].toggle = 0;
								digitalWrite(pgm_read_byte(&motor_config[motorIdx].pul_pin),HIGH);
							}
						}
					}
				}
				else
				{
					motorCtrl[motorIdx].toggle = 1;
					digitalWrite(pgm_read_byte(&motor_config[motorIdx].pul_pin),LOW);
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
	// pinout setup

	for (uint8_t motorIdx = 0; motorIdx < MOTOR_LAST; motorIdx++)
	{
		pinMode(pgm_read_byte(&motor_config[motorIdx].en_pin),OUTPUT);
		pinMode(pgm_read_byte(&motor_config[motorIdx].dir_pin),OUTPUT);
		pinMode(pgm_read_byte(&motor_config[motorIdx].pul_pin),OUTPUT);
		pinMode(pgm_read_byte(&motor_config[motorIdx].calib_pin),INPUT_PULLUP);
	}

	Timer1.initialize(P_BASE);
	Timer1.attachInterrupt(motor_handler);

	// Fill motorsList
	get_motors_list(motorsList, &motorsCount);

}