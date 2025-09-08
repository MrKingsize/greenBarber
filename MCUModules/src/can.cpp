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
#include "common.h"


/*************************** Global Variables ********************************/

MCP_CAN CAN0(&SPI, MCP_CS); // Set CS pin
#define DEBUG_CAN_MESSAGES
IsoTp isotp(&CAN0, MCP_INT);

struct Message_t txMsg, rxMsg;

command_t commandQueue[COMMAND_QUEUE_SIZE] = {0};
volatile uint8_t queueHead = 0;
volatile uint8_t queueTail = 0;

/****************************** Functions *************************************/

void can_init(uint8_t moduleIdx)
{
  
    // Initialize SPI
  SPI.begin();

  // Initialize CAN Bus at 500KBPS (ensure both boards use the same speed!)
  // Remember to use the correct MCP_CLOCK for your module's crystal.
  while (CAN_OK != CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ)) // Ensure MCP_8MHZ is here!
  {
    Serial.println("CAN BUS Shield Init Fail. Retrying...");
    delay(100);
  }
  Serial.println("CAN BUS Shield Init OK!");

  // --- ADD THIS EXPLICIT MODE SET FOR RECEIVER ---
  byte opMode = CAN0.setMode(MCP_NORMAL);
  if (opMode == CAN_OK) {
      Serial.println("Receiver: Successfully set to Normal Mode!");
  } else {
      Serial.print("Receiver: Failed to set to Normal Mode! Error Code: ");
      Serial.println(opMode); // This will print a CAN_FAIL or other error code if it fails
  }

  DEBUG_PRINTF("MCP_INT pin %d set as INPUT_PULLUP\n",MCP_INT);
  DEBUG_PRINTF("MCP_CS pin %d set as OUTPUT\n",MCP_CS);

  txMsg.Buffer = (uint8_t *)calloc(MAX_MSGBUF, sizeof(uint8_t));
  rxMsg.Buffer = (uint8_t *)calloc(MAX_MSGBUF, sizeof(uint8_t));

}

void my_can_receive(void)
{
    // rxMsg.rx_id: This is the CAN ID that *THIS* module (the receiver)
    // is expecting messages to be sent *to*.
    rxMsg.rx_id = ISO_TP_BASE_RX_ID + get_my_module_id();

    // rxMsg.tx_id: This field in the 'rxMsg' struct will be automatically populated
    // by the isotp.receive() function with the *actual CAN ID of the sender*
    // when a message is successfully received. This tells you who sent it.

    if (isotp.receive(&rxMsg) == 0) // isotp.receive() returns 0 on success
    {
        uint8_t canMsg[MESSAGE_LENGTH];
        memcpy(&canMsg, rxMsg.Buffer, sizeof(canMsg));
        uint8_t cmd = canMsg[0];
        uint16_t receivedPayload = (canMsg[1] << 8) | canMsg[2];

        // Enqueue command: Use rxMsg.tx_id (the sender's actual TX CAN ID)
        // to deduce the sender's module ID (rxMsg.tx_id - ISO_TP_BASE_TX_ID).
        enqueue_command(rxMsg.tx_id - ISO_TP_BASE_TX_ID, cmd, receivedPayload);
    }
}

void my_can_send(uint8_t target_module_id, uint8_t cmd, uint16_t payload)
{
    uint8_t canMsg[MESSAGE_LENGTH];
    canMsg[0] = cmd;
    canMsg[1] = (payload >> 8) & 0xFF;
    canMsg[2] = payload & 0xFF;

    txMsg.len = sizeof(canMsg);
    
    // txMsg.rx_id: This is the CAN ID that *THIS* module (the sender)
    // will use to transmit its message.
    txMsg.rx_id = ISO_TP_BASE_TX_ID + get_my_module_id();

    // txMsg.tx_id: This is the CAN ID that the *TARGET* module (the receiver)
    // is listening on for messages specifically addressed to it.
    txMsg.tx_id = ISO_TP_BASE_RX_ID + target_module_id; 

    memcpy(txMsg.Buffer, canMsg, sizeof(canMsg));
    isotp.send(&txMsg);
    //DEBUG_PRINTF("Sent cmd %d to destination %d, payload %d\n", cmd, target_module_id, payload);
    debug_counter_increase();
}

void enqueue_command(uint8_t sender, uint8_t cmd, uint16_t payload)
{
    uint8_t next = (queueHead + 1) % COMMAND_QUEUE_SIZE;

    if (next == queueTail) {
        // Queue full, drop command (or handle overflow)
        DEBUG_PRINTF("Queue full, dropping cmd\n");
        return;
    }

    commandQueue[queueHead].sender_id = sender;
    commandQueue[queueHead].cmd = cmd;
    commandQueue[queueHead].payload = payload;
    TRACE_PRINTF("sender_id %d cmd %d payload %d\n", commandQueue[queueHead].sender_id, commandQueue[queueHead].cmd, commandQueue[queueHead].payload);
    queueHead = next;

    //DEBUG_PRINTF("Enqueing cmd %d to destination %d\n", cmd, sender);
    //DEBUG_PRINTF("queueHead %d queueTail %d\n", queueHead, queueTail);

}

bool dequeue_command(command_t *cmd)
{
    if (queueHead == queueTail) {
        return false;  // Queue is empty
    }

    *cmd = commandQueue[queueTail];
    //memcpy(cmd, &commandQueue[queueTail], sizeof(command_t));
    queueTail = (queueTail + 1) % COMMAND_QUEUE_SIZE;

    //DEBUG_PRINTF("Dequeing cmd %d\n", cmd->cmd);
    //DEBUG_PRINTF("queueHead %d queueTail %d\n", queueHead, queueTail);
    //DEBUG_PRINTF("sender_id %d cmd %d payload %d\n", commandQueue[queueTail].sender_id, commandQueue[queueTail].cmd, commandQueue[queueTail].payload);

    return true;
}


