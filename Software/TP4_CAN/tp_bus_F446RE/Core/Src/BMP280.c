#include "BMP280.h"

// Calibration Storage
uint16_t dig_T1;
int16_t  dig_T2, dig_T3;
uint16_t dig_P1;
int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
BMP280_S32_t t_fine;

void BMP280_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t buffer[25];
    uint8_t reg_addr;

    // 1. Check ID (0xD0)
    reg_addr = BMP280_REG_ID;
    HAL_I2C_Master_Transmit(hi2c, BMP280_I2C_ADDR, &reg_addr, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(hi2c, BMP280_I2C_ADDR, buffer, 1, HAL_MAX_DELAY);
    printf("ID: 0x%02X (Expected: 0x58)\r\n", buffer[0]);

    // 2. Write Config (0xF4 -> 0x57)
    buffer[0] = BMP280_REG_CTRL_MEAS;
    buffer[1] = BMP280_CONFIG_VAL;
    HAL_I2C_Master_Transmit(hi2c, BMP280_I2C_ADDR, buffer, 2, HAL_MAX_DELAY);

    // 3. Read Calibration (Burst Read 24 bytes from 0x88)
    reg_addr = BMP280_REG_CALIB_START;
    HAL_I2C_Master_Transmit(hi2c, BMP280_I2C_ADDR, &reg_addr, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(hi2c, BMP280_I2C_ADDR, buffer, 24, HAL_MAX_DELAY);

    // Assign Calibration Data
    dig_T1 = (buffer[1] << 8) | buffer[0];
    dig_T2 = (int16_t)((buffer[3] << 8) | buffer[2]);
    dig_T3 = (int16_t)((buffer[5] << 8) | buffer[4]);
    dig_P1 = (buffer[7] << 8) | buffer[6];
    dig_P2 = (int16_t)((buffer[9] << 8) | buffer[8]);
    dig_P3 = (int16_t)((buffer[11] << 8) | buffer[10]);
    dig_P4 = (int16_t)((buffer[13] << 8) | buffer[12]);
    dig_P5 = (int16_t)((buffer[15] << 8) | buffer[14]);
    dig_P6 = (int16_t)((buffer[17] << 8) | buffer[16]);
    dig_P7 = (int16_t)((buffer[19] << 8) | buffer[18]);
    dig_P8 = (int16_t)((buffer[21] << 8) | buffer[20]);
    dig_P9 = (int16_t)((buffer[23] << 8) | buffer[22]);
}

void BMP280_Read_Raw(I2C_HandleTypeDef *hi2c, int32_t *adc_P, int32_t *adc_T)
{
    uint8_t reg_addr = BMP280_REG_DATA_START;
    uint8_t data[6];

    // Read 6 bytes starting from 0xF7
    HAL_I2C_Master_Transmit(hi2c, BMP280_I2C_ADDR, &reg_addr, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(hi2c, BMP280_I2C_ADDR, data, 6, HAL_MAX_DELAY);

    // Reassemble 20-bit values
    *adc_P = (int32_t)((data[0] << 12) | (data[1] << 4) | (data[2] >> 4));
    *adc_T = (int32_t)((data[3] << 12) | (data[4] << 4) | (data[5] >> 4));
}

// 32-bit Temp Compensation
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
    BMP280_S32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((BMP280_S32_t)dig_T1 << 1))) * ((BMP280_S32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BMP280_S32_t)dig_T1)) * ((adc_T >> 4) - ((BMP280_S32_t)dig_T1))) >> 12) * ((BMP280_S32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// 64-bit Pressure Compensation
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
    BMP280_S64_t var1, var2, p;
    var1 = ((BMP280_S64_t)t_fine) - 128000;
    var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
    var2 = var2 + ((var1 * (BMP280_S64_t)dig_P5) << 17);
    var2 = var2 + (((BMP280_S64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3) >> 8) + ((var1 * (BMP280_S64_t)dig_P2) << 12);
    var1 = (((((BMP280_S64_t)1) << 47) + var1)) * ((BMP280_S64_t)dig_P1) >> 33;
    if (var1 == 0) return 0;
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((BMP280_S64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((BMP280_S64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7) << 4);
    return (BMP280_U32_t)p;
}
