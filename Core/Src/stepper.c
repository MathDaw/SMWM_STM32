/**
  ******************************************************************************
  * @file    stepper.c
  ******************************************************************************
  * @attention
  *
  * Thank you for your attention
  *
  ******************************************************************************
  */

#include"stepper.h"
#include<main.h>

STEPPER stepper_inizialize(
		GPIO_TypeDef* RefPort, uint16_t RefPin,
		GPIO_TypeDef* DirPort,uint16_t DirPin,
		GPIO_TypeDef* ResetPort,uint16_t ResetPin,
		GPIO_TypeDef* EPort,uint16_t EPin,
		GPIO_TypeDef* Mode1Port,uint16_t Mode1Pin,
		GPIO_TypeDef* Mode2Port,uint16_t Mode2Pin,
		TIM_HandleTypeDef* Timer,int32_t Channel)
{
	//inicjalizacja zmiennej
	STEPPER tmp;
	//przypisanie zmiennych
	tmp.RefPin=RefPin;
	tmp.DirPin=DirPin;
	tmp.ResetPin=ResetPin;
	tmp.EPin=EPin;
	tmp.Mode1Pin=Mode1Pin;
	tmp.Mode2Pin=Mode2Pin;
	tmp.Channel=Channel;
	tmp.Timer=Timer;
	tmp.RefPort=RefPort;
	tmp.DirPort=DirPort;
	tmp.ResetPort=ResetPort;
	tmp.EPort=EPort;
	tmp.Mode1Port=Mode1Port;
	tmp.Mode2Port=Mode2Port;
	tmp.position=0;
	tmp.direction=1; //1 - do przodu, -1 - do tyłu
	tmp.destination[0]=0;
	tmp.destination[1]=0;
	tmp.destination[2]=0;
	tmp.is_working=0;
	tmp.speed=80;
	tmp.state=0;
	//Inicjalizacja pracy zegara
	__HAL_TIM_SET_PRESCALER(Timer,tmp.speed);
	HAL_GPIO_WritePin(tmp.DirPort, tmp.DirPin, 1);
	//rozpoczecie pracy
	HAL_GPIO_WritePin(tmp.ResetPort, tmp.ResetPin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(tmp.Mode1Port, tmp.Mode1Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(tmp.Mode2Port, tmp.Mode2Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(tmp.ResetPort, tmp.ResetPin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(tmp.EPort, tmp.EPin, GPIO_PIN_SET);
	return tmp;
}

void stepper_set_destination(STEPPER* stepper, int32_t pos, uint8_t state)
{
	pos = 12 * pos;
	if(pos > 32000) pos = 3200;

	if(state==1)
	{
		stepper->destination[0]=pos;
		stepper->state=1;
	}
	else if(state==2)
	{
		stepper->destination[0]=pos;
		stepper->destination[1]=4*(pos-stepper->position)/5+stepper->position;
		stepper->destination[2]=(pos-stepper->position)/5+stepper->position;
		//stepper->speed=stepper->speed*2;
		__HAL_TIM_SET_PRESCALER(stepper->Timer, stepper->speed);
		stepper->state=2;

	}

	if(pos>stepper->position)
	{
		stepper->direction=1;
		HAL_GPIO_WritePin(stepper->DirPort, stepper->DirPin, 1);
	}
	else if(pos<stepper->position)
	{
		stepper->direction=-1;
		HAL_GPIO_WritePin(stepper->DirPort, stepper->DirPin, 0);
	}
}

void stepper_set_speed(STEPPER* stepper, int16_t speed)
{
	if(speed < 10) speed = 10;
	else if(speed > 100) speed = 100;
	speed = (int16_t)(143 - 1.41 * speed);
	stepper->speed=speed;
	__HAL_TIM_SET_PRESCALER(stepper->Timer, stepper->speed);
}

void stepper_proceed(STEPPER* stepper)
{
	stepper->is_working=1;
	HAL_GPIO_WritePin(stepper->EPort, stepper->EPin, GPIO_PIN_RESET);
	HAL_TIM_PWM_Start(stepper->Timer, stepper->Channel);
	HAL_TIM_Base_Start_IT(stepper->Timer);
}
