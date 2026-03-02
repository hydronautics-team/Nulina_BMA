#ifndef THRUSTER_H
#define THRUSTER_H


#include "main.h"

#define delay_init 10000
void Thruster_Init(void);
void Thruster_Stop(void);
void Thruster_Set_Speed(uint8_t *speed);

#endif
