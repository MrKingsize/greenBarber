/** @file PS2X.cpp
 * 
 * @brief File for the PS2X controller drivers. 
 *
 */ 

/****************************** Includes *************************************/

#include <Arduino.h>
#include <PS2X_lib.h>  //for v1.6

#include "PS2X.h"
#include "can.h"
#include "module_map.h"
#include "common.h"

/*************************** Global Variables ********************************/
#define DETECT_TRESHOLD 1
#define CONTROLLER_ZERO_POINT 127

int error = -1;
byte type = 0;
byte vibrate = 0;
uint8_t forwardSpeed = 0;
uint8_t dirAngleValue = 0;

PS2X ps2x; // create PS2 Controller Class


/****************************** Functions *************************************/
uint8_t controller_init(void)
{
    TRACE_PRINTF("Entering Function\n");
    delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it
    
    uint8_t controllerFoundFlag = 0;
    //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
    
    if(error == 0)
    {
        Serial.print("Found Controller, configured successful ");
        Serial.print("pressures = ");
        if (pressures)
            Serial.println("true ");
        else
            Serial.println("false");
        Serial.print("rumble = ");
        if (rumble)
            Serial.println("true)");
        else
            Serial.println("false");
        Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
        Serial.println("holding L1 or R1 will print out the analog stick values.");
        Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
    }  
    else if(error == 1)
        Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
    
    else if(error == 2)
        Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

    else if(error == 3)
        Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
      
    Serial.print(ps2x.Analog(1), HEX);
      
    type = ps2x.readType(); 
    switch(type) {
    case 0:
        Serial.print("Unknown Controller type found ");
        break;
    case 1:
        Serial.print("DualShock Controller found ");
        controllerFoundFlag = 1;
        break;
    case 2:
        Serial.print("GuitarHero Controller found ");
        break;
    case 3:
        Serial.print("Wireless Sony DualShock Controller found ");
        break;
    }
    return controllerFoundFlag;
}

void get_controller_cmd(void)
{
    static uint8_t forwardSpeedAnt = CONTROLLER_ZERO_POINT;
    static uint8_t targetDirAnt = CONTROLLER_ZERO_POINT;

    if(error == 1) //skip loop if no controller found
    {
        //controller_init();
        return; 
    }

    //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    
    if (ps2x.NewButtonState())
    {
        if(ps2x.Button(PSB_START)){         //will be TRUE as long as button is pressed
            Serial.println("Start is being held, flush device");
            delay(150); 
            return;
        }
        if(ps2x.Button(PSB_SELECT)){
            Serial.println("Select is being held");
            enqueue_command(MOD_SEL_MCU_MAIN, CMD_CALIBRATE_DIRECTION, 0);
        }    

        
        //if(ps2x.ButtonPressed(PSB_PAD_UP))
        //{
        //    //Serial.println("Up pressed");
        //    enqueue_command(MOD_SEL_MCU_MAIN, CMD_MOVE_FORWARD, 128);
        //}
        //else if (ps2x.ButtonReleased(PSB_PAD_UP))
        //{
        //    //Serial.println("Up released");
        //    enqueue_command(MOD_SEL_MCU_MAIN, CMD_STOP, 0);
        //       
        //}  
//
        //if(ps2x.Button(PSB_PAD_RIGHT)){
        //    //Serial.print("Right held this hard: ");
        //    //Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
        //}
        //if(ps2x.Button(PSB_PAD_LEFT)){
        //    //Serial.print("LEFT held this hard: ");
        //    //Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
        //}
//
        //if(ps2x.ButtonPressed(PSB_PAD_DOWN))
        //{
        //    //Serial.println("Down pressed");
        //    enqueue_command(MOD_SEL_MCU_MAIN, CMD_MOVE_BACKWARDS, 128);
        //}
        //else if (ps2x.ButtonReleased(PSB_PAD_DOWN))
        //{
        //    //Serial.println("Down released");
        //    enqueue_command(MOD_SEL_MCU_MAIN, CMD_STOP, 0);
        //       
        //}   

        if(ps2x.Button(PSB_L3))
            Serial.println("L3 pressed");
        if(ps2x.Button(PSB_R3))
            Serial.println("R3 pressed");
        if(ps2x.Button(PSB_L2))
            Serial.println("L2 pressed");
        if(ps2x.ButtonPressed(PSB_R2))
        {
            Serial.println("R2 pressed, dequeue cmd");
            command_t cmd;
		    dequeue_command(&cmd);
        }
            
        if(ps2x.Button(PSB_TRIANGLE))
            Serial.println("Y pressed");
        if(ps2x.ButtonPressed(PSB_CIRCLE))               //will be TRUE if button was JUST pressed
            Serial.println("B just pressed");
        if(ps2x.NewButtonState(PSB_CROSS))               //will be TRUE if button was JUST pressed OR released
            Serial.println("A just changed");
        if(ps2x.ButtonReleased(PSB_SQUARE))              //will be TRUE if button was JUST released
            Serial.println("X just released");     

    
    }
    if (ps2x.ButtonReleased(PSB_PAD_LEFT))
    {
        //enqueue_command(MOD_SEL_MCU_MAIN, CMD_MOVE_LEFT, 1000);
    }
    if (ps2x.ButtonReleased(PSB_PAD_RIGHT))
    {
        //enqueue_command(MOD_SEL_MCU_MAIN, CMD_MOVE_RIGHT, 1000);
    }
    if (ps2x.ButtonReleased(PSB_PAD_UP))
    {
        //enqueue_command(MOD_SEL_MCU_MAIN, CMD_INCREASE_SPEED, 10);
    }
    if (ps2x.ButtonReleased(PSB_PAD_DOWN))
    {
        //enqueue_command(MOD_SEL_MCU_MAIN, CMD_DECREASE_SPEED, 10);
    }
    if (ps2x.Button(PSB_L1))
    {
        uint8_t speedValueTemp = ps2x.Analog(PSS_RY);
        uint8_t dirValueTemp = ps2x.Analog(PSS_RX);

        //filter controller noise
        if ((speedValueTemp == 255) && (ps2x.Analog(PSS_LY) == 255) && (ps2x.Analog(PSS_LX) == 255) && (dirValueTemp == 255))
        {
            delay(50);
            return;
        }

        forwardSpeed = speedValueTemp; // down -> 255, up -> 0
        dirAngleValue = dirValueTemp; // right -> 255, left -> 0

        //calculate forward speed
        if (abs(forwardSpeedAnt - forwardSpeed) > 10)
        {
            forwardSpeedAnt = forwardSpeed;
            if (forwardSpeed > 128)
            {
                uint8_t targetSpeed = (forwardSpeed - 128)*2;
                enqueue_command(MOD_SEL_MCU_MAIN, CMD_MOVE_BACKWARDS, targetSpeed);

            }
            else if (forwardSpeed < 128)
            {
                uint8_t targetSpeed = (255 - forwardSpeed)*2;
                enqueue_command(MOD_SEL_MCU_MAIN, CMD_MOVE_FORWARD, targetSpeed);
            }
            else
            {
                enqueue_command(MOD_SEL_MCU_MAIN, CMD_STOP, 0);
            }
            
        }

        DEBUG_PRINTF("controller targetDirAnt: %d, dirAngleValue: %d\n", targetDirAnt, dirAngleValue);

        //calculate direction angle
        if ((abs(targetDirAnt - dirAngleValue) > 1) && (dirAngleValue == CONTROLLER_ZERO_POINT))
        {
            targetDirAnt = dirAngleValue;
            enqueue_command(MOD_SEL_MCU_MAIN, CMD_SET_DIRECTION_ANGLE, 0 + CAN_ANGLE_OFFSET);
        }  
        else if (abs(targetDirAnt - dirAngleValue) > DETECT_TRESHOLD)
        {
            targetDirAnt = dirAngleValue;
            double angle = ((dirAngleValue / 255.0) * 90.0) - 45.0;
            uint8_t targetAngleCan = (uint8_t)(angle + CAN_ANGLE_OFFSET);
            enqueue_command(MOD_SEL_MCU_MAIN, CMD_SET_DIRECTION_ANGLE, targetAngleCan);
        }
               
    }

    //if (ps2x.Button(PSB_L1) || ps2x.Button(PSB_PAD_UP) || ps2x.Button(PSB_PAD_DOWN))
    //{
    //    angleTarget = ps2x.Analog(PSS_RX); // right -> 255, left -> 0
    //}
        
    //if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { //print stick values if either is TRUE
    //    Serial.print("Stick Values:");
    //    Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
    //    Serial.print(",");
    //    Serial.print(ps2x.Analog(PSS_LX), DEC); 
    //    Serial.print(",");
    //    Serial.print(ps2x.Analog(PSS_RY), DEC); // down -> 255, up -> 0
    //    Serial.print(",");
    //    Serial.println(ps2x.Analog(PSS_RX), DEC); // right -> 255, left -> 0
    //}   
    
    
}
