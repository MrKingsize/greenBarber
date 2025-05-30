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
#ifdef __AVR_ATmega32U4__
  #define MCP_CS   A0
  #define MCP_INT  8
#elif defined(ARDUINO_AVR_MEGA2560)
  #define MCP_CS   53
  #define MCP_INT  21
#else
  #error "Unsupported board. Define your MCP_CS and MCP_INT pins here."
#endif

// arduino pro micro 
// #define SPI_CLK_PIN 15 
// #define SPI_MISO 14 - MCP2515 SO
// #define SPI_MOSI 16 - MCP2515 SI

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