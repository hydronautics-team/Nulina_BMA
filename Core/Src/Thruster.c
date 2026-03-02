#include "Thruster.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;



void Thruster_Init()
{
	HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_3);


	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_3);


	Thruster_Stop();

}

void Thruster_Stop()
{
	int speed = 150;

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed);

	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, speed);

	HAL_Delay(4000);

}

void Thruster_Set_Speed(uint8_t* speed)
{

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed[2]);//3
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed[1]);//1
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed[0]);//7


	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed[5]);//2
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed[4]);//4
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, speed[3]);//5

}
