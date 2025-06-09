/** @file main.cpp
 * 
 * @brief Main file. 
 * After compiling run the correct target 
 * pio run -e megaatmega2560
 * pio run -e leonardo
 *
 */ 
#include <Arduino.h>

#include "motor.h"
#include "can.h"
#include "module_map.h"
#include "PS2X.h"
#include "common.h"


#include <mcp_can.h>
/*******************************************************************
 * Global Constants
 ******************************************************************/


/*******************************************************************
 * Global Variables
 ******************************************************************/ 

unsigned long previousMillis = 0;
const long interval = 1000; // Send/check every 1 second

/*******************************************************************
 * Prototypes functions
 ******************************************************************/
static void calculate_wheels_angles(uint16_t targetAngle, uint16_t *angleRight, uint16_t *angleLeft);

/******************************************************************************
 * @brief initial setup system's function
 ******************************************************************************/
void setup() {
	Serial.begin(9600);
	unsigned long startTime = millis();
	while (!Serial && (millis() - startTime < 5000));
	
	Serial.print("Setup\n");
	//set_my_module_id(MOD_SEL_HW_X_TRACTION_LEFT);
	//set_my_module_id(MOD_SEL_MCU_MAIN);

	init_my_module_id();



	can_init(get_my_module_id());

	motors_init();

    delay(300);

#ifndef __AVR_ATmega32U4__
	if (get_my_module_id() == MOD_SEL_MCU_MAIN)
		controller_init();
	delay(300);
#endif
}

void debug_function()
{
	if (get_my_module_id() == MOD_SEL_MCU_MAIN)
	{
		my_can_send((uint8_t)MOD_SEL_HW_X_TRACTION_LEFT, (uint8_t)CMD_SET_TRACTION_SPEED_FORWARD_LEFT, 255);
		//debug_send();

	}
	else
	{
		
		my_can_receive();
		//debug_receive();
	}
	command_t cmd;
	if (dequeue_command(&cmd)) {
        // Process the command based on cmd.sender_id, cmd.cmd, cmd.payload
        Serial.print("Dequeued command from sender ");
        Serial.print(cmd.sender_id);
        Serial.print(", Cmd: ");
        Serial.print(cmd.cmd);
        Serial.print(", Payload: ");
        Serial.println(cmd.payload);
    }
	delay(1050);
	
}


/******************************************************************************
 * @brief main loop function
 ******************************************************************************/
void loop()
{
	TRACE_PRINTF("\nEntering Function\n\n");
	// debug loop
	//while (1) {debug_function();}
	//debug_function();
	//while (1);

	uint8_t myModuleIdx = get_my_module_id();

	while(1){
		// can pooling messages
		my_can_receive();

		if (myModuleIdx == MOD_SEL_MCU_MAIN)
			get_controller_cmd();
		
		//get command
		command_t cmd;
		if (dequeue_command(&cmd))
		{
			//DEBUG_PRINTF("Command received: %d\n",cmd.cmd);
			if (myModuleIdx == MOD_SEL_MCU_MAIN)
			{
				if (cmd.cmd == CMD_SET_DIRECTION_ANGLE)
				{
					uint16_t angleRight, angleLeft;
					calculate_wheels_angles(cmd.payload, &angleRight, &angleLeft);

					//send can messages
					//my_can_send((uint8_t)MOD_SEL_HW_DIR_RIGHT, (uint8_t)CMD_SET_DIR_ANGLE_RIGHT, angleRight);
					//my_can_send((uint8_t)MOD_SEL_HW_DIR_LEFT, (uint8_t)CMD_SET_DIR_ANGLE_LEFT, angleLeft);
				}
				else if (cmd.cmd == CMD_MOVE_FORWARD)
				{
					// move both left and right traction motor

					uint16_t speedRight, speedleft;
					uint16_t targetSpeed = cmd.payload;
					// TODO Calculate wheels speed based on current angle
					speedRight = targetSpeed;
					speedleft = targetSpeed;
					
					DEBUG_PRINTF("Action: Move forward! %d\n", targetSpeed);

					// move left motor
					my_can_send((uint8_t)MOD_SEL_HW_X_TRACTION_LEFT, (uint8_t)CMD_SET_TRACTION_SPEED_FORWARD_LEFT, speedleft);
					// move right motor (own)
					motor_control(MOTOR_TRACTION_RIGHT, MOTOR_PLUS_CMD, (uint32_t)speedRight, PWM_MOTOR);

				}
				else if (cmd.cmd == CMD_MOVE_BACKWARDS)
				{
					uint16_t speedRight, speedleft;
					uint16_t targetSpeed = cmd.payload;
					// TODO Calculate wheels speed based on current angle
					speedRight = targetSpeed;
					speedleft = targetSpeed;
					
					DEBUG_PRINTF("Action: Move Backwards! %d\n", targetSpeed);

					// move left motor
					my_can_send((uint8_t)MOD_SEL_HW_X_TRACTION_LEFT, (uint8_t)CMD_SET_TRACTION_SPEED_BACKWARD_LEFT, speedleft);
					// move right motor (own)
					motor_control(MOTOR_TRACTION_RIGHT, MOTOR_MINUS_CMD, speedRight, PWM_MOTOR);

				}
				else if (cmd.cmd == CMD_STOP)
				{
					DEBUG_PRINTF("Action: Stop moving!\n");

					// move left motor
					my_can_send((uint8_t)MOD_SEL_HW_X_TRACTION_LEFT, (uint8_t)CMD_SET_TRACTION_SPEED_FORWARD_LEFT, 0);
					// move right motor (own)
					motor_control(MOTOR_TRACTION_RIGHT, MOTOR_STOP_CMD, 0, PWM_MOTOR);

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
				if (cmd.cmd == CMD_SET_TRACTION_SPEED_FORWARD_LEFT)
				{
					DEBUG_PRINTF("Action: Move forward! %d\n", cmd.payload);
					motor_control(MOTOR_TRACTION_LEFT, MOTOR_PLUS_CMD, (uint32_t)cmd.payload, PWM_MOTOR);

				}if (cmd.cmd == CMD_SET_TRACTION_SPEED_BACKWARD_LEFT)
				{
					DEBUG_PRINTF("Action: Move forward! %d\n", cmd.payload);
					motor_control(MOTOR_TRACTION_LEFT, MOTOR_MINUS_CMD, (uint32_t)cmd.payload, PWM_MOTOR);

				}if (cmd.cmd == CMD_SET_TRACTION_SPEED_STOP_LEFT)
				{
					DEBUG_PRINTF("Action: Move forward! %d\n", cmd.payload);
					motor_control(MOTOR_TRACTION_LEFT, MOTOR_STOP_CMD, (uint32_t)cmd.payload, PWM_MOTOR);

				}
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
		delay(50); 
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


