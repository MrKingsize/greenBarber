/** @file can.cpp
 *
 * @brief File for the CAN communications control using ESP32 TWAI ISO-TP.
 *
 */

/****************************** Includes *************************************/

#include <Arduino.h>
#include "driver/twai.h"
#include "TWAI_ISO.h"

#include "can.h"
#include "module_map.h"

#include "common.h"

/*************************** ISO-TP Context ********************************/
#define CAN_ISOTP_RXBUF_SIZE 256
IsoTpLink_t isotp_link_ctx;
uint8_t isotp_rx_buffer[CAN_ISOTP_RXBUF_SIZE];

/*************************** Global Variables ********************************/

command_t commandQueue[COMMAND_QUEUE_SIZE] = {0};
volatile uint8_t queueHead = 0;
volatile uint8_t queueTail = 0;

#define CAN_RX_PIN_READER GPIO_NUM_22 
#define CAN_TX_PIN_READER GPIO_NUM_21

/****************************** Functions *************************************/

void can_init(uint8_t moduleIdx)
{
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    g_config.rx_queue_len = 20; g_config.tx_queue_len = 10;

    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
        Serial.println("TWAI Driver Install Failed!");
        while (1) delay(10);
    }
    if (twai_start() != ESP_OK) {
        Serial.println("TWAI Start Failed!");
        while (1) delay(10);
    }
    Serial.println("TWAI ISO-TP Init OK!");

    // ISO-TP context setup
    isoTp_initLink(&isotp_link_ctx, isotp_rx_buffer, sizeof(isotp_rx_buffer),
                   1000, 1000, 1000, 1000, 0, 150, 0, 0); // timeouts and params as example
    isoTp_setPaddingByte(&isotp_link_ctx, 0xAA);
}

/**
 * @brief Handle incoming ISO-TP messages and push to command queue.
 */
void my_can_receive(void)
{
    twai_message_t can_frame_rx;
    uint16_t isotp_msg_final_len;
    uint8_t* isotp_msg_final_buf;

    // Poll CAN bus for new frame
    if (twai_receive(&can_frame_rx, pdMS_TO_TICKS(10)) == ESP_OK) {
        // Check if this message is for this module before parsing
        uint16_t expectedId = ISO_TP_BASE_RX_ID + get_my_module_id();
        if (can_frame_rx.identifier == expectedId) {
            // Try to decode ISO-TP message from CAN frame
            if (isoTp_receive(&isotp_link_ctx, &can_frame_rx, &isotp_msg_final_len, &isotp_msg_final_buf)) {
                // Check minimum length for command and payload
                if (isotp_msg_final_len >= 3 && isotp_msg_final_buf != NULL) {
                    uint8_t cmd = isotp_msg_final_buf[0];
                    uint16_t payload = (isotp_msg_final_buf[1] << 8) | isotp_msg_final_buf[2];
                    // Sender module ID logic (customize as needed)
                    uint8_t senderModule = (can_frame_rx.identifier >= ISO_TP_BASE_TX_ID) ? (can_frame_rx.identifier - ISO_TP_BASE_TX_ID) : 0;
                    enqueue_command(senderModule, cmd, payload);
                }
            }
        }
    }
}

/**
 * @brief Send an ISO-TP message to a target module.
 */
void my_can_send(uint8_t target_module_id, uint8_t cmd, uint16_t payload)
{
    uint8_t canMsg[MESSAGE_LENGTH];
    canMsg[0] = cmd;
    canMsg[1] = (payload >> 8) & 0xFF;
    canMsg[2] = payload & 0xFF;

    uint16_t rxId = ISO_TP_BASE_RX_ID + target_module_id;

    // Use isoTp_send from the library
    if (isoTp_send(&isotp_link_ctx, rxId, /*responseId*/ rxId, canMsg, sizeof(canMsg))) {
        DEBUG_PRINTF("Sent cmd %d to destination %d, payload %d\n", cmd, target_module_id, payload);
    } else {
        DEBUG_PRINTF("Failed to send cmd %d to destination %d\n", cmd, target_module_id);
    }
}

/**
 * @brief Enqueue command from received CAN frame.
 */
void enqueue_command(uint8_t sender, uint8_t cmd, uint16_t payload)
{
    uint8_t next = (queueHead + 1) % COMMAND_QUEUE_SIZE;

    if (next == queueTail) {
        DEBUG_PRINTF("Queue full, dropping cmd\n");
        return;  // Queue overflow
    }

    commandQueue[queueHead].sender_id = sender;
    commandQueue[queueHead].cmd = cmd;
    commandQueue[queueHead].payload = payload;

    TRACE_PRINTF("sender_id %d cmd %d payload %d\n",
                 commandQueue[queueHead].sender_id,
                 commandQueue[queueHead].cmd,
                 commandQueue[queueHead].payload);

    queueHead = next;
}

/**
 * @brief Pop one command from queue (FIFO).
 */
bool dequeue_command(command_t *cmd)
{
    if (queueHead == queueTail) {
        return false;  // Queue empty
    }

    *cmd = commandQueue[queueTail];
    queueTail = (queueTail + 1) % COMMAND_QUEUE_SIZE;

    return true;
}
