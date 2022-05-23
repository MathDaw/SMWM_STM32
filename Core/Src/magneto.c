/*
 * magneto.c
 *
 *  Created on: 28 kwi 2022
 *      Author: Lab TMP
 */
#include "magneto.h"
#include "math.h"




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



uint8_t kierunek_kompas(I2C_HandleTypeDef *h){

	uint8_t magneto_status;
	int16_t magneto_z, magneto_x, magneto_y;

	//zmienne kierunkowe
	uint8_t *p_magneto_x = (uint8_t*) &magneto_x;
	uint8_t *p_magneto_y = (uint8_t*) &magneto_y;
	uint8_t *p_magneto_z = (uint8_t*) &magneto_z;

	//czytanie statusu "gotowy do odczytu"
	HAL_I2C_Mem_Read(h, ACC_MAGNETO_ADDR, LIS3MDL_MAG_STATUS_REG, I2C_MEMADD_SIZE_8BIT, &magneto_status, 1, 1);

	//odczyt x y z
	if ( (magneto_status & (1 << 0)) && (magneto_status & (1 << 1)) && (magneto_status & (1 << 2))){

	HAL_I2C_Mem_Read(h, ACC_MAGNETO_ADDR, LIS3MDL_MAG_OUTX_L, I2C_MEMADD_SIZE_8BIT, p_magneto_x, 1, 1);
	HAL_I2C_Mem_Read(h, ACC_MAGNETO_ADDR, LIS3MDL_MAG_OUTX_H, I2C_MEMADD_SIZE_8BIT, p_magneto_x+1, 1, 1);
	HAL_I2C_Mem_Read(h, ACC_MAGNETO_ADDR, LIS3MDL_MAG_OUTY_L, I2C_MEMADD_SIZE_8BIT, p_magneto_y, 1, 1);
	HAL_I2C_Mem_Read(h, ACC_MAGNETO_ADDR, LIS3MDL_MAG_OUTY_H, I2C_MEMADD_SIZE_8BIT, p_magneto_y+1, 1, 1);
	HAL_I2C_Mem_Read(h, ACC_MAGNETO_ADDR, LIS3MDL_MAG_OUTZ_L, I2C_MEMADD_SIZE_8BIT, p_magneto_z, 1, 1);
	HAL_I2C_Mem_Read(h, ACC_MAGNETO_ADDR, LIS3MDL_MAG_OUTZ_H, I2C_MEMADD_SIZE_8BIT, p_magneto_z+1, 1, 1);
	}


	int16_t z_max = Z_MAX, z_min = Z_MIN;
	int16_t y_max = Y_MIN, y_min = Y_MAX;

	// oblcizanie kierunku podanego w katach 0-360
	double z = (double)2.0*(magneto_z-(z_min+z_max)/2.0)/(z_max-z_min);
	double y = (double)2.0*(magneto_y-(y_min+y_max)/2.0)/(y_max-y_min);
	double kat = (M_PI + atan2(z,y));

	/*  przelicznik 360 -> 255  */
	uint8_t kierunek = (uint8_t) 255*(kat/2/M_PI);



	/* sprintf(str, "z=%.2f y=%.2f  ", z, y);
			  BSP_LCD_DisplayStringAtLine(5, (uint8_t *) str);
			  sprintf(str, "%f  ", 180/M_PI*kat);
			  BSP_LCD_DisplayStringAtLine(6, (uint8_t *) str);
			  sprintf(str, "%u  ", kierunek);
			  BSP_LCD_DisplayStringAtLine(7, (uint8_t *) str);
	 */

	return kierunek;

}
