/** @file spiLib.cpp
 * 
 * @brief Main file for spi libs
 *
 */ 


/****************************** Includes *************************************/

#include "spiLib.h"

/*************************** Global Variables ********************************/

volatile byte receivedMessage[MESSAGE_LENGTH];  // Buffer for the received message
volatile byte sendMessage[MESSAGE_LENGTH];  // Message to send
volatile byte currentByte = 0;  // Tracks which byte we're sending/receiving
volatile byte replyByteIndex = 0;
volatile bool replyReady = false;

/****************************** Functions *************************************/

void spi_setup(void){
    pinMode(MISO, OUTPUT);
    SPCR |= _BV(SPE);  // Enable SPI in slave mode
    SPI.attachInterrupt();  // Enable interrupt on SPI transmission complete
    Serial.begin(9600);
}

ISR(SPI_STC_vect)
{
    // Storing received bytes
    byte receivedByte = SPDR;
    receivedMessage[currentByte] = receivedByte;
    currentByte++;

    if (currentByte >= MESSAGE_LENGTH) {
        currentByte = 0;
        memset((void*)&sendMessage, 0, sizeof(sendMessage));
        sendMessage[0] = receivedMessage[0];
        sendMessage[1] = receivedMessage[1];
        
        // Process the received command message
        byte moduleSelection = receivedMessage[0];
        byte command = receivedMessage[1];
        uint16_t commandPayload = (receivedMessage[2] << 8) | receivedMessage[3];


        switch (moduleSelection)
        {
        case MOD_SEL_3_AXIS_BRIDGE:
            // TODO: send command to 3-axis-bridge module
            // Prepare response here, example
            //byte highByte = (payload >> 8) & 0xFF;  // High byte of the reply payload
            //byte lowByte = payload & 0xFF;           // Low byte of the reply payload
            //sendMessage[2] = highByte;  // Send high byte first
            //sendMessage[3] = lowByte;  // Send high byte first
            break;
        case MOD_SEL_TRACTION:
            // TODO: send command to traction module
            break;
        case MOD_SEL_DIRECTION:
            // TODO: send command to module
            break;
        case MOD_SEL_POWER:
            // TODO: send command to module
            break;
        
        default:
            sendMessage[2] = ERROR_INVALID_MODULE;
            break;
        }

        Serial.print("Module Selection: ");
        Serial.println(moduleSelection);
        Serial.print("Command: ");
        Serial.println(command);
        Serial.print("Command Payload: ");
        Serial.println(commandPayload);

        
        // Prepare to send the response
        replyByteIndex = 0;
        replyReady = true;
        SPDR = sendMessage[replyByteIndex++];  // Start sending the first byte    
    }
    else if (replyReady && replyByteIndex < MESSAGE_LENGTH)
    {
        // Continue sending the remaining reply bytes
        SPDR = sendMessage[replyByteIndex++];
        if (replyByteIndex >= MESSAGE_LENGTH)
        {
            // All reply bytes sent, reset for the next transfer
            replyReady = false;
        }
    }
}

/*******************************************************************************
 * @brief print dos valores de controladores dos motores
 *******************************************************************************/
