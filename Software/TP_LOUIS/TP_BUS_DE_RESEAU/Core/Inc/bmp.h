/*
 * bmp.h
 *
 *  Created on: Nov 24, 2025
 *      Author: louisvoz
 */

#ifndef INC_BMP_H_
#define INC_BMP_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>

// I2C Address (Shifted for HAL)
#define BMP280_I2C_ADDR         (0x77 << 1)

// Registers
#define BMP280_REG_ID           0xD0
#define BMP280_REG_CALIB_START  0x88
#define BMP280_REG_CTRL_MEAS    0xF4
#define BMP280_REG_DATA_START   0xF7

// Config: osrs_t x2, osrs_p x16, normal mode
#define BMP280_CONFIG_VAL       0x57

// Bosch Data Types
typedef int32_t  BMP280_S32_t;
typedef uint32_t BMP280_U32_t;
typedef int64_t  BMP280_S64_t;

// Global Calibration Variables
extern uint16_t dig_T1;
extern int16_t  dig_T2, dig_T3;
extern uint16_t dig_P1;
extern int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
extern BMP280_S32_t t_fine;

// Functions
void BMP280_Init(I2C_HandleTypeDef *hi2c);
void BMP280_Read_Raw(I2C_HandleTypeDef *hi2c, int32_t *adc_P, int32_t *adc_T);
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P);

#endif /* INC_BMP_H_ */
