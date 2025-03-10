/** @file can.h
 * 
 * @brief Header for the can communications control. 
 *
 */ 


#ifndef CAN_ENABLED
#define CAN_ENABLED

/****************************** Includes *************************************/



/****************************** Constants *************************************/

// All modules need to use the same CAN pinout
#define MCP_CS 18   // A0
#define MCP_INT 19  // A1

// arduino pro micro 
// #define SPI_CLK_PIN 15 
// #define SPI_MISO 14 - MCP2515 SO
// #define SPI_MOSI 16 - MCP2515 SI

#define MESSAGE_LENGTH 3  // Define how many bytes per message
#define COMMAND_QUEUE_SIZE 5

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

#endif