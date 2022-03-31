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

/*
 * #define MOT_REF_Pin GPIO_PIN_7
#define MOT_REF_GPIO_Port GPIOC
#define MOT_DIR1_Pin GPIO_PIN_8
#define MOT_DIR1_GPIO_Port GPIOA
#define MOT_RESET_Pin GPIO_PIN_9
#define MOT_RESET_GPIO_Port GPIOA
#define MOT_FAULT_Pin GPIO_PIN_10
#define MOT_FAULT_GPIO_Port GPIOA
#define MOT_PWM1_Pin GPIO_PIN_3
#define MOT_PWM1_GPIO_Port GPIOB
#define MOT_MODE1_Pin GPIO_PIN_4
#define MOT_MODE1_GPIO_Port GPIOB
#define MOT_MODE2_Pin GPIO_PIN_6
#define MOT_MODE2_GPIO_Port GPIOB
 */

struct {
	GPIO_TypeDef* DirPort;
	uint16_t DirPin;
	GPIO_TypeDef* ResetPort;
	uint16_t ResetPin;
	GPIO_TypeDef* FaultPort;
	uint16_t FaultPin;
	GPIO_TypeDef* Mode1Port;
	uint16_t Mode1Pin;
	GPIO_TypeDef* Mode2Port;
	uint16_t Mode2Pin;
	TIM_TypeDef* Timer;
	uint32_t Channel;

} STEPPER;

STEPPER stepper_inizialize();
