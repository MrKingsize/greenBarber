/** @file can.h
 * 
 * @brief Header for the can communications control. 
 *
 */ 


#ifndef CAN_ENABLED
#define CAN_ENABLED

/****************************** Includes *************************************/



/****************************** Constants *************************************/

#define MESSAGE_LENGTH 3  // Define how many bytes per message
#define COMMAND_QUEUE_SIZE 5
#define ISO_TP_BASE_RX_ID   0x100 // Base for individual module receive IDs
#define ISO_TP_BASE_TX_ID   0x200 // Base for individual module transmit IDs

/****************************** Structures *************************************/



struct command_t {
    uint8_t sender_id;
    uint8_t cmd;
    uint16_t payload;
};

/****************************** Function Prototypes *************************************/
bool dequeue_command(command_t *cmd);
void enqueue_command(uint8_t sender, uint8_t cmd, uint16_t payload);
void my_can_receive(void);
void my_can_send(uint8_t module, uint8_t cmd, uint16_t payload);
void can_init(uint8_t moduleIdx);
//void debug_receive(void);
//void debug_send(void);
#endif