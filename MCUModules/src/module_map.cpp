/** @file module_id.cpp
 * 
 * @brief File with functions related to the module ID 
 *
 */ 


/****************************** Includes *************************************/
#include <Arduino.h>

#include "module_map.h"


/*************************** Global Variables ********************************/

uint8_t myModuleIdx; 

/****************************** Functions *************************************/

/******************************************************************************
 * @brief Initilizes ID, identifies itself
 ******************************************************************************/
void init_my_module_id(void)
{
    // TODO get ID from flash storage
    uint8_t myModuleID = 0;

    myModuleIdx = myModuleID - 1;
    
    
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
void get_motors_list(uint8_t *motorList, uint8_t *motorsCount)
{
    const struct module_cmd_t *mod = &moduleMap[myModuleIdx]; // Get the struct
    const uint8_t *motorsListPtr = (const uint8_t *)pgm_read_ptr(&mod->motorsList);


    *motorsCount = pgm_read_byte(&mod->cmdListCount);
    
    for (uint8_t motorIdx = 0; motorIdx < *motorsCount; motorIdx++)
    {
        motorList[motorIdx] = pgm_read_byte(&motorsListPtr[motorIdx]);
    }
}