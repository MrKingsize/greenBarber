/** @file can.cpp
 * 
 * @brief File for the can communications control. 
 *
 */ 

/****************************** Includes *************************************/

#include <Arduino.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>
#include <iso-tp.h>

#include "can.h"
#include "module_map.h"


/*************************** Global Variables ********************************/

MCP_CAN CAN0(MCP_CS);
IsoTp isotp(&CAN0, MCP_INT);

struct Message_t txMsg, rxMsg;

command_t commandQueue[COMMAND_QUEUE_SIZE];
volatile uint8_t queueHead = 0;
volatile uint8_t queueTail = 0;

/****************************** Functions *************************************/

void can_init(MOD_SEL_ENUM_E moduleIdx)
{
    pinMode(MCP_INT, INPUT);
    CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ);
    CAN0.setMode(MCP_NORMAL);

    txMsg.Buffer = (uint8_t *)calloc(MAX_MSGBUF, sizeof(uint8_t));
    rxMsg.Buffer = (uint8_t *)calloc(MAX_MSGBUF, sizeof(uint8_t));

    // Configure Timer3: 16-bit timer, CTC mode, Prescaler 64
    //noInterrupts();  // Disable interrupts during setup
    //TCCR3A = 0;      // Clear register
    //TCCR3B = 0;
    //TCCR3B |= (1 << WGM32) | (1 << CS31) | (1 << CS30); // CTC Mode, Prescaler 64
    //OCR3A = 24999;   // Timer Compare Value (100ms Interrupt)
    //TIMSK3 |= (1 << OCIE3A); // Enable Timer3 Compare Interrupt
    //interrupts();  // Re-enable interrupts
}

//ISR(TIMER3_COMPA_vect) {
//    my_can_receive();
//}


void my_can_receive(void)
{
    rxMsg.tx_id = get_my_module_id() + 1;

    for (uint8_t i = 0; i < MODULE_NUM; i++)
    {
        rxMsg.rx_id = i;
        if (isotp.receive(&rxMsg) == 0)
        {
            uint8_t canMsg[MESSAGE_LENGTH];
            memcpy(&canMsg, rxMsg.Buffer,sizeof(canMsg));

            uint8_t cmd = canMsg[0];
            uint16_t receivedPayload = (canMsg[1] << 8) | canMsg[2];

            // enqueue command to proccess
            enqueue_command(rxMsg.rx_id, cmd, receivedPayload);
        }
    }    
}

void my_can_send(uint8_t module, uint8_t cmd, uint16_t payload)
{
    uint8_t canMsg[MESSAGE_LENGTH];
    canMsg[0] = cmd;
    
    // Convert value to big-endian format (MSB first)
    canMsg[1] = (payload >> 8) & 0xFF;  // High byte
    canMsg[2] = payload & 0xFF;         // Low byte

    txMsg.len = sizeof(canMsg);
    txMsg.tx_id = module + 1;
    txMsg.rx_id = get_my_module_id() + 1;
    memcpy(txMsg.Buffer,canMsg,sizeof(canMsg));
    isotp.send(&txMsg);
}

void enqueue_command(uint8_t sender, uint8_t cmd, uint16_t payload)
{
    uint8_t next = (queueHead + 1) % COMMAND_QUEUE_SIZE;

    if (next == queueTail) {
        // Queue full, drop command (or handle overflow)
        return;
    }

    commandQueue[queueHead].sender_id = sender;
    commandQueue[queueHead].cmd = cmd;
    commandQueue[queueHead].payload = payload;
    queueHead = next;
}

bool dequeue_command(command_t *cmd)
{
    if (queueHead == queueTail) {
        return false;  // Queue is empty
    }

    *cmd = commandQueue[queueTail];
    queueTail = (queueTail + 1) % COMMAND_QUEUE_SIZE;
    return true;
}

