/** @file module_id.cpp
 * 
 * @brief File with functions related to the module ID 
 *
 */ 


/****************************** Includes *************************************/
#include <Arduino.h>
#include <EEPROM.h>

#include "module_map.h"
#include "common.h"

/*************************** Global Variables ********************************/

uint8_t myModuleIdx; 
#define DEFAULT_MODULE_ID   1

/****************************** Functions *************************************/

/******************************************************************************
 * @brief Initilizes ID, identifies itself
 ******************************************************************************/
void init_my_module_id(void)
{
    TRACE_PRINTF("Entering Function\n");
    // TODO get ID from flash storage
    myModuleIdx = EEPROM.read(0);  // Assuming ID is at address 0

    DEBUG_PRINTF("My Module: %d\n", myModuleIdx);

    if (myModuleIdx == 0xFF || myModuleIdx == 0 || myModuleIdx > MODULE_NUM) {
        TRACE_PRINTF("Invalid ID read from EEPROM: %u\n", myModuleIdx);
        myModuleIdx = DEFAULT_MODULE_ID;  // Set a fallback or enter safe mode
    }
      
}

/******************************************************************************
 * @brief Writes own ID into EEPROM memory
 ******************************************************************************/
void set_my_module_id(uint8_t id)
{
    DEBUG_PRINTF("Setting module ID: %d\n",id);
    EEPROM.write(0, id);  // Write the ID to address 0
}

/******************************************************************************
 * @brief returns own ID
 ******************************************************************************/
uint8_t get_my_module_id(void)
{
    return myModuleIdx; 
}

/******************************************************************************
 * @brief Returns true if Motor control handler
 ******************************************************************************/
void get_motors_list(uint8_t *motorList, uint8_t *motorsCountStepper, uint8_t *motorListPWM, uint8_t *motorsCountPWM)
{
    const struct module_cmd_t *mod = &moduleMap[myModuleIdx]; // Get the struct
    const uint8_t *motorsListPtr = (const uint8_t *)pgm_read_ptr(&mod->motorsList);

    uint8_t motorsCountTemp = pgm_read_byte(&mod->motorsCount);
    uint8_t motorListTemp[motorsCountTemp];
    uint8_t motorListPWMTemp[motorsCountTemp];
    uint8_t motorsCountStepperTemp = 0;
    uint8_t motorsCountPWMTemp = 0;

    for (uint8_t motorIdx = 0; motorIdx < motorsCountTemp; motorIdx++)
    {
        //DEBUG_PRINTF("motor_config[%d].motorIdx=%d\n",motorIdx,pgm_read_byte(&motor_config[motorIdx].motorIdx));
        //DEBUG_PRINTF("motor_config[%d].motorType=%d\n",motorIdx,pgm_read_byte(&motor_config[motorIdx].motorType));

        if (pgm_read_byte(&motor_config[motorIdx].motorType) != PWM_MOTOR)
        {
            motorListTemp[motorsCountStepperTemp++] = pgm_read_byte(&motorsListPtr[motorIdx]);
            
        }
        else
        {
            motorListPWMTemp[motorsCountPWMTemp++] = pgm_read_byte(&motorsListPtr[motorIdx]);
            
        }
    }
    *motorsCountStepper = motorsCountStepperTemp;
    *motorsCountPWM = motorsCountPWMTemp;
    
    //DEBUG_PRINTF("motorsCountStepperTemp=%d, motorsCountPWMTemp=%d\n",motorsCountStepperTemp,motorsCountPWMTemp);
	//DEBUG_PRINTF("motorsCountStepper=%d, motorsCountPWM=%d\n",*motorsCountStepper,*motorsCountPWM);

    memcpy(motorList, &motorListTemp, motorsCountStepperTemp);
    memcpy(motorListPWM, &motorListPWMTemp, motorsCountPWMTemp);

    //for (uint8_t i = 0; i < motorsCountStepperTemp; i++)
    //    motorList[i] = motorListTemp[i];
    //for (uint8_t i = 0; i < motorsCountPWMTemp; i++)
    //    motorListPWM[i] = motorListPWMTemp[i];
    
	//DEBUG_PRINTF("motorsCountStepperTemp=%d, motorsCountPWMTemp=%d\n",motorsCountStepperTemp,motorsCountPWMTemp);
	//DEBUG_PRINTF("motorsCountStepper=%d, motorsCountPWM=%d\n",*motorsCountStepper,*motorsCountPWM);
}