/** @file monitor.cpp
 * 
 * @brief File to control the inter modules communication monitor system. 
 *
 */ 

/****************************** Includes *************************************/
#include <Arduino.h>
#include <util/atomic.h>
#include "monitor.h"
#include "common.h"
#include "module_map.h"
#include "can.h"

/*************************** Global Variables ********************************/
#ifdef MODULE_ID_MOD_SEL_MCU_MAIN
struct modulestate_t moduleState[MODULE_NUM] = {0};

/****************************** Functions *************************************/


/******************************************************************************
 * @brief sets the current state of another module for the monitoring system
 * moduleId: module index
 * state: new state
 *******************************************************************************/
void set_mod_mon_state(uint8_t state, uint8_t moduleId)
{
    moduleState[moduleId].moduleState = state;
}


/******************************************************************************
 * @brief initializes the monitoring system
 ******************************************************************************/
void mon_init(void)
{
    if (get_my_module_id() == MOD_SEL_MCU_MAIN)
    {
        // Initialize all module states
        for (uint8_t i = 2; i < MODULE_NUM; i++)
        {
            moduleState[i].moduleState = STATE_INIT;
            moduleState[i].previousModuleState = STATE_INIT;
            moduleState[i].last_ping_time_ms = 0;
        }
    }
}

/******************************************************************************
 * @brief controls the monitoring system, has to be called periodically
 * returns a bitmask with the connected modules
 ******************************************************************************/
uint8_t mon_ctrl(void)
{
    static uint16_t loopCounter = 0;
    static uint8_t moduleConnectedBits = 0x00;

    if (loopCounter * TOTAL_MAIN_LOOP_DELAY >= PING_INTERVAL_MS)
    {
        DEBUG_PRINTF("Pinging modules\n");
        moduleConnectedBits = 0x00;
        loopCounter = 0;
        //my_can_send((uint8_t)3, (uint8_t)CMD_COMMS_PING, 0); // Ping the MCU_BMS first as it is critical
        for (uint8_t i = 2; i < MODULE_NUM; i++)
        {
           
            //my_can_send((uint8_t)i, (uint8_t)CMD_COMMS_PING, 0);
            if (moduleState[i].moduleState == STATE_COMMS_CONNECTED)
            {
                moduleConnectedBits |= (1 << i);

                if (moduleState[i].previousModuleState != moduleState[i].moduleState)
                {
                    moduleState[i].last_ping_time_ms = 0;
                    moduleState[i].previousModuleState = moduleState[i].moduleState;
                    DEBUG_PRINTF("Module %d state changed to COMMS_CONNECTED\n", i);
                }
            }
            else if (moduleState[i].moduleState == STATE_COMMS_NOT_CONNECTED)
            {
                if (moduleState[i].previousModuleState != moduleState[i].moduleState)
                {
                    moduleConnectedBits |= (1 << i);
                    moduleState[i].previousModuleState = moduleState[i].moduleState;
                    DEBUG_PRINTF("Module %d state changed to COMMS_NOT_CONNECTED\n", i);
                }
                else
                {
                    moduleState[i].last_ping_time_ms += TOTAL_MAIN_LOOP_DELAY;
                    if (moduleState[i].last_ping_time_ms >= PING_TIMEOUT_MS)
                    {
                        // timeout, do not set Connected bit
                    }
                    else
                    {
                        moduleConnectedBits |= (1 << i);
                    }
                }
            }
        }

    }
        
    loopCounter++;
    return moduleConnectedBits;
}
#endif