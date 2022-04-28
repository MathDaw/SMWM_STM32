/*
 * magneto.c
 *
 *  Created on: 28 kwi 2022
 *      Author: Lab TMP
 */
#include "magneto.h"

void cfg_magneto(I2C_HandleTypeDef *h){

	uint8_t i2c2_buf[10];
	i2c2_buf[0] = 0x00;
	HAL_I2C_Mem_Write(h, ACC_MAGNETO_ADDR, LIS3MDL_MAG_CTRL_REG2, I2C_MEMADD_SIZE_8BIT, i2c2_buf, 1, 1);
	i2c2_buf[0] = 0xFC;
	HAL_I2C_Mem_Write(h, ACC_MAGNETO_ADDR, LIS3MDL_MAG_CTRL_REG1, I2C_MEMADD_SIZE_8BIT, i2c2_buf, 1, 1);
	i2c2_buf[0] = 0x0C;
	HAL_I2C_Mem_Write(h, ACC_MAGNETO_ADDR, LIS3MDL_MAG_CTRL_REG4, I2C_MEMADD_SIZE_8BIT, i2c2_buf, 1, 1);
	i2c2_buf[0] = 0x00;
	HAL_I2C_Mem_Write(h, ACC_MAGNETO_ADDR, LIS3MDL_MAG_CTRL_REG3, I2C_MEMADD_SIZE_8BIT, i2c2_buf, 1, 1);
}

