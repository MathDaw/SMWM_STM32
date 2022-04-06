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

STEPPER* stepper_inizialize(
		GPIO_TypeDef* RefPort, uint16_t RefPin,
		GPIO_TypeDef* DirPort,uint16_t DirPin,
		GPIO_TypeDef* ResetPort,uint16_t ResetPin,
		GPIO_TypeDef* FaultPort,uint16_t FaultPin,
		GPIO_TypeDef* Mode1Port,uint16_t Mode1Pin,
		GPIO_TypeDef* Mode2Port,uint16_t Mode2Pin,
		TIM_TypeDef* Timer,int32_t Channel)
{
	//inicjalizacja zmiennej
	STEPPER* tmp;
	//przypisanie zmiennych
	tmp->RefPin=RefPin;
	tmp->DirPin=DirPin;
	tmp->ResetPin=ResetPin;
	tmp->FaultPin=FaultPin;
	tmp->Mode1Pin=Mode1Pin;
	tmp->Mode2Pin=Mode2Pin;
	tmp->Channel=Channel;
	tmp->Timer=Timer;
	tmp->RefPort=RefPort;
	tmp->DirPort=DirPort;
	tmp->ResetPort=ResetPort;
	tmp->FaultPort=FaultPort;
	tmp->Mode1Port=Mode1Port;
	tmp->Mode2Port=Mode2Port;
	tmp->position=0;
	tmp->direction=1; //1 - do przodu, -1 - do tyłu
	tmp->destination=0;
	tmp->is_working=0;
	//Inicjalizacja pracy zegara
	HAL_TIM_Base_Start_IT(tmp->Timer);
	HAL_GPIO_WritePin(tmp->DirPort, tmp->DirPin, 1);
	return tmp;
}

void stepper_set_destination(STEPPER* stepper, int32_t pos)
{
	stepper->destination=pos;
	if(pos>stepper->position)
		stepper->direction=1;
	else if(pos<stepper->position)
		stepper->direction=-1;
}

uint8_t stepper_is_working(STEPPER* stepper)
{
	return stepper->is_working;
}

void stepper_proceed(STEPPER* stepper)
{
	stepper->is_working=1;
	HAL_TIM_PWM_Start(stepper->Timer, stepper->Channel);
}
