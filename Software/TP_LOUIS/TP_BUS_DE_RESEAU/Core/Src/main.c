/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "bmp.h"
#include "ADXL.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define SIZE_BUFFER 1

uint32_t value[2];
uint8_t rxData[SIZE_BUFFER];

uint8_t parser[6];

Measure meas = {0};

int temp = 0;
int pres = 0;

float temp1 = 0;
float pres1 = 0;

uint8_t compteur = 0;

uint32_t txMailbox;

uint8_t angle_reel;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

	return ch;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  uint8_t buf = 0xD0;
  uint8_t buffer = 0;

  if (HAL_I2C_Master_Transmit(&hi2c1, ADRESS_BMP , &buf, 1, 100) == HAL_OK)
  {
      if (HAL_I2C_Master_Receive(&hi2c1,ADRESS_BMP, &buffer, 1, 100) == HAL_OK)
      {
      }
  }
  BMP280_Init(&hi2c1);
  ADXL343_Init();
  HAL_UART_Receive_IT(&huart1, &rxData, SIZE_BUFFER);
  //----------------CAN-----------------//

  if (HAL_CAN_Start (&hcan1) != HAL_OK)
  {
	  Error_Handler();
  }
  CAN_TxHeaderTypeDef header = {
		  .StdId = 0x60, //contient le message ID quand celui-ci est standard (11 bits)
		  .ExtId = 0x0, //contient le message ID quand celui-ci est étendu (29 bits)
		  .IDE = CAN_ID_STD, //définit si la trame est standard (CAN_ID_STD) ou étendue (CAN_ID_EXT)
		  .RTR = CAN_RTR_DATA, //définit si la trame est du type standard (CAN_RTR_DATA) ou RTR (CAN_RTR_REMOTE) (voir le cours)
		  .DLC = 3, //entier représentant la taille des données à transmettre (entre 0 et 8)
		  .TransmitGlobalTime = DISABLE //dispositif permettant de mesurer les temps de réponse du bus CAN, qu'on utilisera pas. Le fixer à DISABLE
  };


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //angle(0xB0,0x01);
	  //manual_mode(0x01,0xB4,0x01);
	  BMP280_Read_Raw(&hi2c1, &pres, &temp);
	  temp1 = bmp280_compensate_T_int32(temp)/100.0;
	  pres1 = bmp280_compensate_P_int64(pres)/25600.0;

	  printf("temp : %.2f",temp1);
	  process_temperature(temp1);
	  HAL_Delay(100);

	  //angle(0x0,0x00);

	  HAL_Delay(10);
	    //read_value(&value);
	  //ADXL343_ReadXYZ(&meas);
	  //temp = read_temperature_uint();
	  //pres = read_pressure_uint();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void PARSE(uint8_t* buffer,uint8_t size)
{
	for (int i = 0;i<size;i++)
	{
		if (buffer[i]=='G' && buffer[i+1]=='E' && buffer[i+2]=='T' && buffer[i+3]=='_')
		{
			if (buffer[i + 4] == 'T')
			{
				char ligne[20];
				int len = snprintf(ligne, sizeof(ligne), "Valeur de T:%lu \r\n", value[1]);
				HAL_UART_Transmit(&huart1, (uint8_t*)ligne, len, 10);

			}
			if (buffer[i + 4] == 'P')
			{
				char ligne[20];
				int len = snprintf(ligne, sizeof(ligne), "Valeur de P:%lu\r\n", value[0]);
				HAL_UART_Transmit(&huart1, (uint8_t*)ligne, len, 10);
			}
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)   // vérifier la bonne UART
    {

        // --- Relancer la réception pour le prochain octet ---
    	if (compteur < 5)
    	{
    		parser[compteur]=rxData[0];
    		compteur = compteur + 1;


    	}
    	else
    	{
    		parser[compteur]=rxData[0];
    		compteur = 0;
    		PARSE(&parser,6);


    		//HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 10);
    	}

        HAL_UART_Receive_IT(&huart1, &rxData,SIZE_BUFFER );
    }
}

void manual_mode(uint8_t rotation,uint8_t step,uint8_t speed)
{
	CAN_TxHeaderTypeDef header = {
			  .StdId = 0x60, //contient le message ID quand celui-ci est standard (11 bits)
			  .ExtId = 0x0, //contient le message ID quand celui-ci est étendu (29 bits)
			  .IDE = CAN_ID_STD, //définit si la trame est standard (CAN_ID_STD) ou étendue (CAN_ID_EXT)
			  .RTR = CAN_RTR_DATA, //définit si la trame est du type standard (CAN_RTR_DATA) ou RTR (CAN_RTR_REMOTE) (voir le cours)
			  .DLC = 3, //entier représentant la taille des données à transmettre (entre 0 et 8)
			  .TransmitGlobalTime = DISABLE //dispositif permettant de mesurer les temps de réponse du bus CAN, qu'on utilisera pas. Le fixer à DISABLE
	};
	uint8_t manual[3]={rotation,step,speed};
	if (HAL_CAN_AddTxMessage (&hcan1, &header,manual, &txMailbox) != HAL_OK)
	  {
		  Error_Handler();
	  }
}

void angle(uint8_t angle1,uint8_t sens)
{
	CAN_TxHeaderTypeDef header = {
			  .StdId = 0x61, //contient le message ID quand celui-ci est standard (11 bits)
			  .ExtId = 0x0, //contient le message ID quand celui-ci est étendu (29 bits)
			  .IDE = CAN_ID_STD, //définit si la trame est standard (CAN_ID_STD) ou étendue (CAN_ID_EXT)
			  .RTR = CAN_RTR_DATA, //définit si la trame est du type standard (CAN_RTR_DATA) ou RTR (CAN_RTR_REMOTE) (voir le cours)
			  .DLC = 2, //entier représentant la taille des données à transmettre (entre 0 et 8)
			  .TransmitGlobalTime = DISABLE //dispositif permettant de mesurer les temps de réponse du bus CAN, qu'on utilisera pas. Le fixer à DISABLE
	};
	uint8_t manual[2]={angle1,sens};
	if (HAL_CAN_AddTxMessage (&hcan1, &header,manual, &txMailbox) != HAL_OK)
	  {
		  Error_Handler();
	  }
}

float old_pos = 0.0f;
uint8_t motor_speed = 0x01;  // vitesse fixe, ajustable

void process_temperature(float temp)
{
    float delta = temp - old_pos;

    if (delta == 0.0f)
        return;  // rien à faire si stable

    uint16_t new_pos = (temp - 20.0f) / (0.047f) ;

    uint16_t steps = new_pos - old_pos ;

    if (steps > 0)
    {
        // Température augmente → moteur horaire
        manual_mode(0x01, steps, motor_speed);
    }
    else
    {
        // Température baisse → moteur anti-horaire
        manual_mode(0x00, steps, motor_speed);
    }

    old_pos = new_pos; // mise à jour
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
