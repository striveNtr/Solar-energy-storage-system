/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "adc_sampling.h"
#include "tim_sg90.h"
#include "iic_ina226.h"
#include "spi_tft144.h"
#include "spi_tft144_gui.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char dis0[25];//液晶显示暂存数组
char dis1[25];//液晶显示暂存数组

#define MyLCD_Show(m,n,p)     LCD_ShowString(LCD_GetPos_X(F_SIZE,m),LCD_GetPos_Y(F_SIZE,n),p,F_SIZE,false)

#define RONGCHAZHI_UD  12 //容差值
#define RONGCHAZHI_LR  12 //容差值

#define ZHUANDONG_ZZ  50 //转动一个正角度
#define ZHUANDONG_FZ  -50 //转动另一个方向转动

int lighVla_left = 0;//ad采集结果	
int lighVla_up = 0;//ad采集结果
int lighVla_right = 0;//ad采集结果
int lighVla_down = 0;//ad采集结果
float batteryVolt = 0;//锂电池电压ad采集结果
float BatCap=0.8;			  //容量初始化


unsigned char disFlag = 0;//更新显示
unsigned char setMode =0;//设置模式
unsigned char rememberMode =0xff;//记录上一次设置状态

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_ADCEx_Calibration_Start(&hadc1);
	
	INA226_init();
	LCD_Init();
  FRONT_COLOR=Color16_LIGHTGRAY;int	disYplace=0; //显示在行递增变量	Color16_WHITE	//Color16_LIGHTGRAY
  MyLCD_Show(2,disYplace++,"Solar charge");//显示
  FRONT_COLOR=Color16_LIGHTBLUE;
  MyLCD_Show(1,disYplace++,"Light par: ");//显示
  MyLCD_Show(4,disYplace++,"up: ");//显示
  MyLCD_Show(1,disYplace,"lt: ");//显示
  MyLCD_Show(9,disYplace++,"rt: ");//显示
  MyLCD_Show(4,disYplace++,"dn: ");//显示
  // MyLCD_Show(1,disYplace++,"Set: ");//显示
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	
	uint32_t display_tick_update = HAL_GetTick();
  uint32_t sg90_tick_update = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		HAL_Delay(2);
		//1.调整旋转方向
		if(sg90_tick_update - HAL_GetTick() > 14)//更新显示
		{
				sg90_tick_update = HAL_GetTick();
				if((lighVla_left - lighVla_right)>RONGCHAZHI_LR ) //调整向强光方向转动
				{sg90_RotateAngle(1,ANGLE_SUBTRACT);}
				else if((lighVla_right - lighVla_left )>RONGCHAZHI_LR )//调整向强光方向转动
				{sg90_RotateAngle(1,ANGLE_ADD);}

				if((lighVla_up - lighVla_down)>RONGCHAZHI_UD ) //调整向强光方向转动
				{sg90_RotateAngle(2,ANGLE_SUBTRACT);}
				else if((lighVla_down - lighVla_up )>RONGCHAZHI_UD )//调整向强光方向转动
				{sg90_RotateAngle(2,ANGLE_ADD);}
		}
		//2.计算光强
		lighVla_left =	1000*ADC_Get_Average(4,1)/4096;
		//1000 - 1000*ADC_Get_Average(4,1)/4096; //读取ad值 转化为0-1000
		lighVla_up = 1000*ADC_Get_Average(0,1)/4096;
		//1000 - 1000*ADC_Get_Average(0,1)/4096; //读取ad值 转化为0-1000
		lighVla_right =	1000*ADC_Get_Average(5,1)/4096;
	  //1000 - 1000*ADC_Get_Average(5,1)/4096; //读取ad值 转化为0-1000
    lighVla_down = 1000*ADC_Get_Average(1,1)/4096;
    //1000 - 1000*ADC_Get_Average(1,1)/4096; //读取ad值 转化为0-1000
		
		//3.更新显示
		if(display_tick_update - HAL_GetTick() > 1000)//更新显示
	{
    	display_tick_update = HAL_GetTick();

		FRONT_COLOR=Color16_LIGHTBLUE;	disYplace=2; //显示所在行递增变量
		sprintf(dis0,"up:%d  ",lighVla_up);//打印
		MyLCD_Show(4,disYplace++,dis0);//显示

		sprintf(dis0,"%d ",lighVla_left);//打印
		MyLCD_Show(4,disYplace,dis0);//显示
		sprintf(dis0,"%d ",lighVla_right);//打印
		MyLCD_Show(12,disYplace++,dis0);//显示

		sprintf(dis0,"%d ",lighVla_down);//打印
		MyLCD_Show(7,disYplace++,dis0);//显示

		batteryVolt  = INA226_GetBusV();//锂电池电压

		if(batteryVolt>4.15)	//电压值对比
		{BatCap = 0.99;}//容量
		else if(batteryVolt<3.4)
		{BatCap =0;}
		else
		{BatCap = (batteryVolt-3.4)/(4.15-3.4);}//正常情况下计算比例

		if(BatCap<0.45){FRONT_COLOR=Color16_RED;	}
		sprintf(dis0,"B:%3.1fv Q:%02d%% ",batteryVolt,(int)(BatCap*100));		//打印
		MyLCD_Show(1,6,dis0);//显示
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
