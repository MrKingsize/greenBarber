/** @file can.cpp
 *
 * @brief File for the CAN communications control using ESP32 TWAI ISO-TP.
 *
 */

/****************************** Includes *************************************/

#include <Arduino.h>
#include "driver/twai.h"

#include "can.h"
#include "module_map.h"

#include "common.h"

/*************************** Global Variables ********************************/

command_t commandQueue[COMMAND_QUEUE_SIZE] = {0};
volatile uint8_t queueHead = 0;
volatile uint8_t queueTail = 0;

#define CAN_RX_PIN_READER GPIO_NUM_16
#define CAN_TX_PIN_READER GPIO_NUM_17

/****************************** Functions *************************************/

void can_init(uint8_t moduleIdx)
{
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN_READER, CAN_RX_PIN_READER, TWAI_MODE_NORMAL);
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
}

/**
 * @brief Handle incoming ISO-TP messages and push to command queue.
 */
void my_can_receive(void)
{
    twai_message_t can_frame_rx;

    if (twai_receive(&can_frame_rx, pdMS_TO_TICKS(10)) == ESP_OK) {
        // You would no longer check for ISO-TP frame types
        uint16_t expectedId = ISO_TP_BASE_RX_ID + get_my_module_id();

        if (can_frame_rx.identifier == expectedId && can_frame_rx.data_length_code >= 3) {
            uint8_t cmd = can_frame_rx.data[0];
            uint16_t payload = (can_frame_rx.data[1] << 8) | can_frame_rx.data[2];
            
            // Your application logic to handle the received command
            enqueue_command(get_my_module_id(), cmd, payload);
        }
    }
}

/**
 * @brief Send an ISO-TP message to a target module.
 */
void my_can_send(uint8_t target_module_id, uint8_t cmd, uint16_t payload)
{
    twai_message_t message;
    message.identifier = ISO_TP_BASE_RX_ID + target_module_id; // Or a new ID scheme
    message.data_length_code = 3;
    message.data[0] = cmd;
    message.data[1] = (payload >> 8) & 0xFF;
    message.data[2] = payload & 0xFF;

    // Zero out the unused bytes for clarity
    message.data[3] = 0;
    message.data[4] = 0;
    message.data[5] = 0;
    message.data[6] = 0;
    message.data[7] = 0;

    if (twai_transmit(&message, pdMS_TO_TICKS(100)) == ESP_OK) {
        //DEBUG_PRINTF("Sent cmd %d to destination %d, payload %d\n", cmd, target_module_id, payload);
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
