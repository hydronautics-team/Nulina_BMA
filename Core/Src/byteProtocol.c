#include "byteProtocol.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;

static uint8_t rx_dma_buffer[RX_BUFFER_SIZE];
static uint8_t ring_buffer[RING_BUFFER_SIZE];
static volatile uint16_t ring_head = 0;
static volatile uint16_t ring_tail = 0;

static volatile uint32_t packet_count   = 0;
static volatile uint32_t error_count    = 0;
static volatile uint32_t bytes_received = 0;
static volatile uint32_t error_range_count = 0;

uint8_t data[MOTORS_COUNT] = {150,150,150,150,150,150};

static uint16_t ring_available(void) {
    if (ring_head >= ring_tail) return ring_head - ring_tail;
    return RING_BUFFER_SIZE - ring_tail + ring_head;
}

static uint16_t ring_free(void) {
    return RING_BUFFER_SIZE - ring_available() - 1;
}

static void ring_push(const uint8_t *data, uint16_t len) {
    if (len > ring_free()) {
        error_count++;
        return;
    }
    for (uint16_t i = 0; i < len; i++) {
        ring_buffer[ring_head++] = data[i];
        if (ring_head == RING_BUFFER_SIZE) ring_head = 0;
    }
}


uint8_t Calculate_CRC8(const uint8_t *data, uint16_t length) {
    uint8_t crc = 0;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
            crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : crc << 1;
    }
    return crc;
}


static void process_complete_packet(uint8_t* buffer) {

    uint8_t received_crc = buffer[PACKET_SIZE - 1];
    uint8_t computed_crc = Calculate_CRC8(&buffer[HEADER_SIZE], DATA_SIZE);

    if (received_crc != computed_crc) {
        error_count++;
        return;
    }

    uint8_t *motor_bytes = &buffer[HEADER_SIZE];


    for (int i = 0; i < 6; i++) {
        uint8_t byte_val = motor_bytes[i];
        if (byte_val >= 100 && byte_val <= 200) {
        	data[i] = byte_val;
        } else {
            error_range_count++;
        }
    }

    packet_count++;
}


static void parse_ring_buffer(void) {

    while (ring_available() >= PACKET_SIZE) {

        uint16_t h1_idx = ring_tail;
        uint16_t h2_idx = (ring_tail + 1) % RING_BUFFER_SIZE;


        if (ring_buffer[h1_idx] != HEADER_BYTE_1 || ring_buffer[h2_idx] != HEADER_BYTE_2) {
            ring_tail = (ring_tail + 1) % RING_BUFFER_SIZE;
            continue;
        }


        uint8_t packet[PACKET_SIZE];
        for (int i = 0; i < PACKET_SIZE; i++) {
            packet[i] = ring_buffer[(ring_tail + i) % RING_BUFFER_SIZE];
        }

        process_complete_packet(packet);

        ring_tail = (ring_tail + PACKET_SIZE) % RING_BUFFER_SIZE;
    }
}


void ByteProtocol_Init(void) {
    memset(rx_dma_buffer, 0, sizeof(rx_dma_buffer));
    ring_head = ring_tail = 0;
    packet_count = 0;
    error_count = 0;
    bytes_received = 0;

    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_dma_buffer, RX_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);

}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance != USART2) return;
    bytes_received += Size;

    ring_push(rx_dma_buffer, Size);
    parse_ring_buffer();

}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART2) return;

    uint32_t error = HAL_UART_GetError(huart);

    if (error & HAL_UART_ERROR_ORE){

        error_count++;
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);


        volatile uint8_t dummy __attribute__((unused));
        dummy = huart->Instance->RDR;
    }

    if (error & (HAL_UART_ERROR_FE | HAL_UART_ERROR_NE | HAL_UART_ERROR_PE)){

        error_count++;
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE | UART_FLAG_NE | UART_FLAG_PE);
    }

    HAL_UART_DMAStop(huart);
    //HAL_Delay(1);
    ByteProtocol_Init();
}
