/*
 * bmp.c
 *
 *  Created on: Nov 24, 2025
 *      Author: louisvoz
 */
#include "main.h"
#include "i2c.h"

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} bmp280_calib_data;

bmp280_calib_data calib;
extern I2C_HandleTypeDef hi2c1;

#define BMP280_ADDR   (0x76 << 1)   // Ou 0x77 << 1 selon ta carte

#define pres_lsb 0xF8
#define pres_msb 0xF7

#define temp_lsb 0xFB
#define temp_msb 0xFA


void INIT_BMP()
{
	uint8_t calibration[24];

	uint8_t adress[2] = {0xD0,0};
	uint8_t value = 0;
	if (HAL_I2C_Master_Transmit(&hi2c1, ADRESS_BMP,(uint8_t*)&adress, 1, 100) == HAL_OK)
	{
	    if (HAL_I2C_Master_Receive(&hi2c1, ADRESS_BMP, &value, 1, 100) == HAL_OK)
	    {
	    	if (value != 0x58)
	    	{
	    		Error_Handler();
	    	}
	    }
	}
	adress[0]=0xF4;//(address)register ctrl_meas
	adress[1]=0b11001011; //(value) Config mode normal + oversample P *16 + pversample T *2
	if (HAL_I2C_Master_Transmit(&hi2c1,ADRESS_BMP,(uint8_t*) &adress, sizeof(adress), 100) == HAL_OK)
		{
		if (HAL_I2C_Master_Receive(&hi2c1, ADRESS_BMP, &value, 1, 100) == HAL_OK)
			    {
			    	if (value != adress[1])
			    	{
			    		Error_Handler();
			    	}
			    }
		}
	adress[0]= 0x88; // compensation register
	adress[1]=0;
	if (HAL_I2C_Master_Transmit(&hi2c1, ADRESS_BMP,(uint8_t*) &adress, sizeof(adress), 100) == HAL_OK)
			{
			if (HAL_I2C_Master_Receive(&hi2c1, ADRESS_BMP,(uint8_t*) &calibration, sizeof(calibration), 100) == HAL_OK)
					{
						// Remplir la structure avec les bons types et endian
						calib.dig_T1 = (uint16_t)(calibration[1] << 8 | calibration[0]);
						calib.dig_T2 = (int16_t)(calibration[3] << 8 | calibration[2]);
						calib.dig_T3 = (int16_t)(calibration[5] << 8 | calibration[4]);

						calib.dig_P1 = (uint16_t)(calibration[7] << 8 | calibration[6]);
						calib.dig_P2 = (int16_t)(calibration[9] << 8 | calibration[8]);
						calib.dig_P3 = (int16_t)(calibration[11] << 8 | calibration[10]);
						calib.dig_P4 = (int16_t)(calibration[13] << 8 | calibration[12]);
						calib.dig_P5 = (int16_t)(calibration[15] << 8 | calibration[14]);
						calib.dig_P6 = (int16_t)(calibration[17] << 8 | calibration[16]);
						calib.dig_P7 = (int16_t)(calibration[19] << 8 | calibration[18]);
						calib.dig_P8 = (int16_t)(calibration[21] << 8 | calibration[20]);
						calib.dig_P9 = (int16_t)(calibration[23] << 8 | calibration[22]);
					}
			}

}
int32_t t_fine = 0;
void read_value(uint32_t* buffer)
{
	uint8_t value[6];
	uint8_t adress = pres_msb;
	if (HAL_I2C_Master_Transmit(&hi2c1,ADRESS_BMP,(uint8_t*) &adress,sizeof(adress), 100) == HAL_OK)
	{
		if (HAL_I2C_Master_Receive(&hi2c1, ADRESS_BMP, &value, 6, 100) == HAL_OK)
				{
					buffer[0] = (value[0] << 12) | (value[1] << 4) | (value[2] >> 4);//pression
					buffer[1] = (value[3] << 12) | (value[4] << 4) | (value[5] >> 4);//temperature
				}
	}

}

uint32_t read_temperature_uint(void)
{
    uint8_t reg = 0xFA; // TEMP_MSB
    uint8_t data[3];

    HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1, 100);
    HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, data, 3, 100);

    int32_t adc_T = ((uint32_t)data[0] << 12) |
                    ((uint32_t)data[1] << 4)  |
                    (data[2] >> 4);

    int32_t var1 = ((((adc_T >> 3) - ((int32_t)calib.dig_T1 << 1))) *
                    ((int32_t)calib.dig_T2)) >> 11;

    int32_t var2 = (((((adc_T >> 4) - ((int32_t)calib.dig_T1)) *
                      ((adc_T >> 4) - ((int32_t)calib.dig_T1))) >> 12) *
                    ((int32_t)calib.dig_T3)) >> 14;

    t_fine = var1 + var2;

    uint32_t T = (t_fine * 5 + 128) >> 8;

    return T;
}

uint32_t read_pressure_uint(void)
{
    uint8_t reg = 0xF7; // PRESS_MSB
    uint8_t data[3];

    HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, &reg, 1, 100);
    HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, data, 3, 100);

    uint32_t adc_P = ((uint32_t)data[0] << 12) |
                     ((uint32_t)data[1] << 4)  |
                     (data[2] >> 4);

    int64_t var1, var2, p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)calib.dig_P4) << 35);

    var1 = ((var1 * var1 * (int64_t)calib.dig_P3) >> 8) +
           ((var1 * (int64_t)calib.dig_P2) << 12);

    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calib.dig_P1) >> 33;

    if (var1 == 0) return 0; // éviter division par zéro

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;

    var1 = (((int64_t)calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)calib.dig_P7) << 4);

    uint32_t pressure = (uint32_t)(p / 256); // pression en Pa

    return pressure;
}
