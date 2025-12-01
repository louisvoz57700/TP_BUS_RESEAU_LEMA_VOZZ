/*
 * sensor.h
 *
 *  Created on: Oct 28, 2025
 *      Author: louisvoz
 */

#ifndef INC_ADXL_H_
#define INC_ADXL_H_

#include "main.h"

typedef struct{
	float A_X;
	float A_Y;
	float A_Z;
} Measure;


HAL_StatusTypeDef ADXL343_Init(void);
HAL_StatusTypeDef ADXL343_ReadReg(uint8_t reg, uint8_t *value);
HAL_StatusTypeDef ADXL343_WriteReg(uint8_t reg, uint8_t value);
HAL_StatusTypeDef ADXL343_ReadXYZ(Measure *m1);
HAL_StatusTypeDef ADXL343_EnableSingleTap(void);


#endif /* INC_ADXL_H_ */
