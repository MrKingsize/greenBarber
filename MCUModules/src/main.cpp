/** @file main.cpp
 * 
 * @brief Main file. 
 * After compiling run the correct target 
 * pio run -e megaatmega2560
 * pio run -e leonardo
 *
 */ 
#include <Arduino.h>

#include <TimerOne.h>

#include "motor.h"
#include "can.h"
#include "module_map.h"
#include "PS2X.h"

/*******************************************************************
 * Local Constants
 ******************************************************************/


/*******************************************************************
 * Local Variables
 ******************************************************************/ 


/*******************************************************************
 * Prototypes functions
 ******************************************************************/
static void calculate_wheels_angles(uint16_t targetAngle, uint16_t *angleRight, uint16_t *angleLeft);


/******************************************************************************
 * @brief initial setup system's function
 ******************************************************************************/
void setup() {
	Serial.begin(115200);

	init_my_module_id();
	motors_init();
	controller_init();

}


/******************************************************************************
 * @brief main loop function
 ******************************************************************************/
void loop()
{
	uint8_t myModuleIdx = get_my_module_id();

	while(1){
		// can pooling messages
		my_can_receive();

		get_controller_cmd();
		
		//get command
		command_t cmd;
		if (dequeue_command(&cmd))
		{
			if (myModuleIdx == MOD_SEL_MCU_MAIN)
			{
				if (cmd.cmd == CMD_SET_DIRECTION_ANGLE)
				{
					uint16_t angleRight, angleLeft;
					calculate_wheels_angles(cmd.payload, &angleRight, &angleLeft);

					//send can messages
					my_can_send((uint8_t)MOD_SEL_HW_DIR_RIGHT, (uint8_t)CMD_SET_DIR_ANGLE_RIGHT, angleRight);
					my_can_send((uint8_t)MOD_SEL_HW_DIR_LEFT, (uint8_t)CMD_SET_DIR_ANGLE_LEFT, angleLeft);
				}
				
			}
			else if (myModuleIdx == MOD_SEL_HW_DIR_RIGHT)
			{

			}
			else if (myModuleIdx == MOD_SEL_HW_DIR_LEFT)
			{

			}
			else if (myModuleIdx == MOD_SEL_HW_X_TRACTION_LEFT)
			{

			}
			else if (myModuleIdx == MOD_SEL_HW_Y)
			{

			}
			else if (myModuleIdx == MOD_SEL_HW_Z)
			{

			}
			else if (myModuleIdx == MOD_SEL_HW_CUT)
			{

			}
		
		}
	}
  
}


/******************************************************************************
 * @brief Calculates Left and right wheel angles from on a target angle
 * when the right wheel is at 50º the left has to be at 60.3º and target angle 56º
 * positive values of angles are to the right and negative are to the left
 * 50 = INTERNAL_WHEEL_FACTOR*56
 * 60.3 = EXTERNAL_WHEEL_FACTOR*56
 * @note because the can payload is a uint16_t and the angle values can range between negative and positive
 * we add an offset of CAN_ANGLE_OFFSET
 ******************************************************************************/
static void calculate_wheels_angles(uint16_t targetAngle, uint16_t *angleRight, uint16_t *angleLeft)
{
	int16_t singedTargetAngle = targetAngle - CAN_ANGLE_OFFSET;
	int16_t singedAngleRight, singedAngleLeft;

	if (singedTargetAngle > 0)
	{
		singedAngleRight = (int16_t)((double)INTERNAL_WHEEL_FACTOR * singedTargetAngle);
		singedAngleLeft = (int16_t)((double)EXTERNAL_WHEEL_FACTOR * singedTargetAngle);
	}
	else
	{
		singedAngleRight = (int16_t)((double)EXTERNAL_WHEEL_FACTOR * singedTargetAngle);
		singedAngleLeft = (int16_t)((double)INTERNAL_WHEEL_FACTOR * singedTargetAngle);
	}

	*angleRight = singedAngleRight + CAN_ANGLE_OFFSET;
	*angleLeft = singedAngleLeft + CAN_ANGLE_OFFSET;
}


