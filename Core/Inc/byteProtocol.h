#ifndef BYTE_PROTOCOL_H
#define BYTE_PROTOCOL_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

/* Headers */
#define HEADER_BYTE_1         0xFF
#define HEADER_BYTE_2         0xFD

#define HEADER_SIZE           2
#define MOTORS_COUNT          6
#define DATA_SIZE             MOTORS_COUNT
#define CRC_SIZE              1

#define PACKET_SIZE           (HEADER_SIZE + DATA_SIZE + CRC_SIZE)

#define RX_BUFFER_SIZE        512
#define RING_BUFFER_SIZE      1024

/* Function Prototypes */
void ByteProtocol_Init(void);
void ByteProtocol_enter(uint8_t motor_idx,uint8_t rpm);
uint8_t Calculate_CRC8(const uint8_t *data, uint16_t length);

extern uint8_t data[MOTORS_COUNT];


#endif
