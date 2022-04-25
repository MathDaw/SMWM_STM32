/**
  ******************************************************************************
  * @file    stepper.h
  ******************************************************************************
  * @attention
  *
  * Thank you for your attention
  *
  ******************************************************************************
  */

#include<main.h>
#define STEPPER_STAY 0;
#define STEPPER_RECTANGLE 1;
#define STEPPER_TRAPEZE 2;


typedef struct {
	//pins
	GPIO_TypeDef* RefPort;
	uint16_t RefPin;
	GPIO_TypeDef* DirPort;
	uint16_t DirPin;
	GPIO_TypeDef* ResetPort;
	uint16_t ResetPin;
	GPIO_TypeDef* EPort;
	uint16_t EPin;
	GPIO_TypeDef* Mode1Port;
	uint16_t Mode1Pin;
	GPIO_TypeDef* Mode2Port;
	uint16_t Mode2Pin;
	//timer
	TIM_HandleTypeDef* Timer;
	int32_t Channel;
	//state
	int32_t position;
	int32_t destination[3];
	uint8_t is_working;
	int8_t direction;
	uint16_t speed;
	uint8_t state;
} STEPPER;

STEPPER stepper_inizialize(
		GPIO_TypeDef* RefPort, uint16_t RefPin,
		GPIO_TypeDef* DirPort,uint16_t DirPin,
		GPIO_TypeDef* ResetPort,uint16_t ResetPin,
		GPIO_TypeDef* EPort,uint16_t EPin,
		GPIO_TypeDef* Mode1Port,uint16_t Mode1Pin,
		GPIO_TypeDef* Mode2Port,uint16_t Mode2Pin,
		TIM_HandleTypeDef* Timer,int32_t Channel);

void stepper_set_destination(STEPPER* stepper, int32_t pos, uint8_t state);

void stepper_proceed(STEPPER* stepper);

void stepper_set_speed(STEPPER* stepper, int16_t speed);
