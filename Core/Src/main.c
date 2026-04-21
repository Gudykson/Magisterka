/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdio.h>

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
CAN_HandleTypeDef hcan;

/* USER CODE BEGIN PV */
uint8_t Kombinacja = 0;
uint8_t flaga_zmiany_zegara = 0;
uint8_t flaga_zmiany_CAN = 0;
uint8_t flaga_zmiany_DLC = 0;
uint32_t CAN_Prescaler;
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
CAN_TxHeaderTypeDef tempTxHeader;
uint32_t TxMailbox;
uint8_t TxData[8] = {0x55, 0x55, 0x55, 0x55,
                     0x55, 0x55, 0x55, 0x55};
uint8_t RxData[8];
uint8_t temp_TxData[8];
uint32_t StartTick = 0;
uint32_t Delay = 1000;
uint32_t tick = 0; 
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
/* USER CODE BEGIN PFP */
void Przestaw_zegar(uint8_t Kombinacja);
void Przestaw_CAN(uint32_t CAN_Prescaler);
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
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */

  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
  // Start the CAN peripheral
  HAL_CAN_Start(&hcan);
  // Configure initial CAN message header and data
  TxHeader.StdId = 0xf; // Standard ID
  TxHeader.DLC = 8; // Data length
  TxHeader.IDE = CAN_ID_STD; // Standard ID
  TxHeader.RTR = CAN_RTR_DATA; // Data frame
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // Transmit the CAN message
    tick = HAL_GetTick();
    if(tick - StartTick >= Delay)
    {
      StartTick = HAL_GetTick();
      HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
    }
    if (flaga_zmiany_zegara) 
    {
      Przestaw_zegar(Kombinacja);
      flaga_zmiany_zegara = 0;
    }
    if (flaga_zmiany_CAN) 
    {
      Przestaw_CAN(CAN_Prescaler);
      flaga_zmiany_CAN = 0;
    }
    if (flaga_zmiany_DLC) 
    {
      TxHeader.DLC = RxData[0]; // DLC can only be between 0 and 8
      flaga_zmiany_DLC = 0;
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */
  
  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 5;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
  CAN_FilterTypeDef CanFilterConfig;

  CanFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
  CanFilterConfig.FilterBank = 0;
  CanFilterConfig.FilterFIFOAssignment = CAN_FilterFIFO0;
  CanFilterConfig.FilterIdHigh = 0xFF0 << 5;       
  CanFilterConfig.FilterIdLow = 0;
  CanFilterConfig.FilterMaskIdHigh = 0;  
  CanFilterConfig.FilterMaskIdLow = 0;
  CanFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  CanFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  
  if (HAL_CAN_ConfigFilter(&hcan, &CanFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB12 PB13 PB14
                           PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC6 PC7 PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief Function to switch the system clock to a different frequency (8 MHz, 16 MHz, 32 MHz, or 64 MHz)
  * @param MHz: Desired system clock frequency in MHz (8, 16, 32, or 64)
  * @retval None
*/ 
void Przestaw_zegar(uint8_t Kombinacja)
{  
  RCC_OscInitTypeDef RCC_OscInitStruct = {0}; // Struktura do nowej konfiguracji oscylatorów
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}; // Struktura do nowej konfiguracji zegarów
  RCC_OscInitTypeDef RCC_OscInitStructOld = {0}; // Struktura do przechowania oryginalnej konfiguracji oscylatorów
  RCC_ClkInitTypeDef RCC_ClkInitStructOld = {0}; // Struktura do przechowania oryginalnej konfiguracji zegarów
  uint32_t pFlatency;


  /*
    Procedura:  
        - Odczytanie aktualnej konfiguracji
        - Włącz HSI, HSE i PLL (HAL_RCC_OscConfig) 
        - Ustaw wybrany zegar jako źródło SYSCLK (HAL_RCC_ClockConfig)
        - 
        - Przestawienie źródła MCO na SYSCLK (nie będzie tego w programie właściwym)
        - Przestawienie CANa na nowy zegar dla case 10, 11
        - Przestawienie źródła MCO
        
  */

  // Odczytanie aktualnej konfiguracji
  HAL_RCC_GetClockConfig(&RCC_ClkInitStructOld, &pFlatency);
  HAL_RCC_GetOscConfig(&RCC_OscInitStructOld);

  RCC_ClkInitStruct = RCC_ClkInitStructOld;
  RCC_OscInitStruct = RCC_OscInitStructOld;

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
  // Włącz HSI, HSE i PLL
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  //RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;


  // Ustaw HSI jako źródło SYSCLK (jeśli nie jest) i wszystkie dzielniki na 1
  if(RCC_ClkInitStruct.SYSCLKSource != RCC_SYSCLKSOURCE_HSI)
  {
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
      Error_Handler();
    }
  }
  // Przestawienie źródła MCO na SYSCLK (nie będzie tego w programie właściwym)
  // HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
  // Wyłączenie PLLki bo nie można zmienić jej ustawień gdy jest włączona

  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  // No i tera można se ustawiać co się chce

  switch (Kombinacja) 
  {
    case 0: // Minimalna emisja - Tylko HSI (PLL wyłączone) - punkt odniesienia

      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  

      break;

    case 1: // PLL włączone ale nie wykorzystane jako źródło SYSCLK

      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;


      break;

    case 2: // Zbadanie wpływu samej PLLki jako wejścia zegarowego przy utrzymaniu tej samej częstotliwości zegarów
    
      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      break;

    case 3: // Wpływ (HSI/2) * PLL_mul = SYSClk w casach od 3 do 5

      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

      break;

    case 4: // Wpływ (HSI/2) * PLL_mul = SYSClk w casach od 3 do 5

      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

      break;  

    case 5: // Wpływ (HSI/2) * PLL_mul = SYSClk w casach od 3 do 5

      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV8;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

      break;

    case 6: // Wpływ HCLK + HSI (pkt 5 jako odniesienie)

      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  
      break;

    case 7:

      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;

      break;
    
    case 8:

      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;

      break;

    case 9: // Nie działa bo APB1 max 36 MHz, a tu byłoby 64 MHz - Wpływ samego APB1 i APB2

      // RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      // RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      // RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      // RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      // RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
      // RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;

      // RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      // RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      // RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

      break;

    case 10: // Wpływ samego APB1 i APB2

      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;

      break;

    case 11: // Wpływ samego APB1 i APB2

      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;

      break;

    case 12: // Wpływ samego włączenia HSE bez podłączenia do PLL i źródło sysclk dalej HSI (odnieść do pktu 0)

      RCC_OscInitStruct.HSEState = RCC_HSE_ON;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

      break;

    case 13: // Podłączenie HSE jako wejścia SYSCLK (bez PLLki)
      RCC_OscInitStruct.HSEState = RCC_HSE_ON;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

      break;

    case 14: // Wyłączenie HSI i wykorzystanie HSE zamiast niego jako wejścia SYSCLK (Porównać z pkt 0)
      // Edge case. Wyłączenie HSI bez wcześniejszego włączenia HSE i Ustawienia jako źródła SYSCLK wywala HAL_ERROR
      // Włącz HSE i ustaw HSE jako źródło SYSCLK    
      RCC_OscInitStruct.HSEState = RCC_HSE_ON;
      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
      if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
      {
        Error_Handler();
      }
      if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
      {
        Error_Handler();
      }    
      RCC_OscInitStruct.HSEState = RCC_HSE_ON;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

      break;

    case 15: // HSE przez PLL jako źródło SYSCLK (dzielone przez 2 żeby sysclk był 8)
      // Edge case. Wyłączenie HSI bez wcześniejszego włączenia HSE i Ustawienia jako źródła SYSCLK wywala HAL_ERROR
      // Włącz HSE i ustaw HSE jako źródło SYSCLK
      RCC_OscInitStruct.HSEState = RCC_HSE_ON;
      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
      if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
      {
        Error_Handler();
      }
      if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
      {
        Error_Handler();
      }
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
      RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

      break;
    
    case 16: // Konfiguracja bazowa do testów bitrate CANa
      
      // Edge case. Wyłączenie HSI bez wcześniejszego włączenia HSE i Ustawienia jako źródła SYSCLK wywala HAL_ERROR
      RCC_OscInitStruct.HSEState = RCC_HSE_ON;
      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
      if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
      {
        Error_Handler();
      }
      if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
      {
        Error_Handler();
      }    
      RCC_OscInitStruct.HSEState = RCC_HSE_ON;
      RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
      RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
      RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  } 

  // Ustawienie APB2CLKDivider na tę samą wartość co APB1CLKDivider
  RCC_ClkInitStruct.APB2CLKDivider = RCC_ClkInitStruct.APB1CLKDivider;

  int CLK_IN = 8; // MHz - częstotliwość wejściowa zegara (HSI lub HSE, w zależności od konfiguracji)
  int sysclk_freq = 0;
  
  // Obliczenie mnożnika PLL
  int PLL_multiplier = 1;
  if(RCC_OscInitStruct.PLL.PLLState == RCC_PLL_ON)
  {
    switch (RCC_OscInitStruct.PLL.PLLMUL) 
    {
      case RCC_PLL_MUL2:
        PLL_multiplier = 2;
        break;
      case RCC_PLL_MUL3:
        PLL_multiplier = 3;
        break;
      case RCC_PLL_MUL4:
        PLL_multiplier = 4;
        break;
      case RCC_PLL_MUL5:
        PLL_multiplier = 5;
        break;
      case RCC_PLL_MUL6:
        PLL_multiplier = 6;
        break;
      case RCC_PLL_MUL7:
        PLL_multiplier = 7;
        break;
      case RCC_PLL_MUL8:
        PLL_multiplier = 8;
        break;    
      case RCC_PLL_MUL9:
        PLL_multiplier = 9;
        break;    
      case RCC_PLL_MUL10:
        PLL_multiplier = 10;
        break;    
      case RCC_PLL_MUL11:
        PLL_multiplier = 11;
        break;    
      case RCC_PLL_MUL12:
        PLL_multiplier = 12;
        break;    
      case RCC_PLL_MUL13:
        PLL_multiplier = 13;
        break;    
      case RCC_PLL_MUL14:
        PLL_multiplier = 14;
        break;    
      case RCC_PLL_MUL15:
        PLL_multiplier = 15;
        break;    
      case RCC_PLL_MUL16:
        PLL_multiplier = 16;
        break;    
    }
  }
  // Przypisanie wartości rzeczywistej dzielnika HSEPrediv
  int HSEPrediv = 1;
  if(RCC_OscInitStruct.HSEPredivValue == RCC_HSE_PREDIV_DIV1)
  {
    HSEPrediv = 1;
  }
  else if(RCC_OscInitStruct.HSEPredivValue == RCC_HSE_PREDIV_DIV2)
  {
    HSEPrediv = 2;
  }

  // Obliczenie częstotliwości SYSCLK w oparciu o aktualną konfigurację
  switch (RCC_ClkInitStruct.SYSCLKSource)
  {
    case RCC_SYSCLKSOURCE_HSI: // sysclk = HSI
      sysclk_freq = CLK_IN;
      break;    
    case RCC_SYSCLKSOURCE_HSE: // sysclk = HSE  
      sysclk_freq = CLK_IN;
      break;
    case RCC_SYSCLKSOURCE_PLLCLK: // sysclk = PLLCLK, PLLCLK = [(HSI/2) * pllmul or (HSE / HSEPrediv) * pllmul]
      if (RCC_OscInitStruct.PLL.PLLSource == RCC_PLLSOURCE_HSI_DIV2) 
      {
        sysclk_freq = (CLK_IN / 2) * PLL_multiplier;
      }
      else if(RCC_OscInitStruct.PLL.PLLSource == RCC_PLLSOURCE_HSE) 
      {
        sysclk_freq = (CLK_IN / HSEPrediv) * PLL_multiplier;
      }
      break;  
  }

  /*
  - 0 wait states, if 0 < SYSCLK <= 24 MHz
  - 1 wait state, if 24 MHz < SYSCLK <= 48 MHz
  - 2 wait states, if 48 MHz < SYSCLK <= 72 MHz
  */
  if (sysclk_freq <= 24) {
    pFlatency = FLASH_LATENCY_0;
  } else if (sysclk_freq <= 48) {
    pFlatency = FLASH_LATENCY_1;
  } else {
    pFlatency = FLASH_LATENCY_2;
  }




  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFlatency) != HAL_OK)
  {
    Error_Handler();
  }  

  // if(sysclk_freq >= 48)
  // {
  //   HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_1);
  // }
  // else {
  //   HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
  // }


  // Dla kombinacji 10 prescaler = 20, dla 11 i 16 prescaler = 10 w celu utrzymania stałego bitrate
  if(Kombinacja == 10)
  {
    hcan.Init.Prescaler = 20;
    HAL_CAN_Stop(&hcan);
    if (HAL_CAN_Init(&hcan) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_CAN_Start(&hcan);
  }
  else if(Kombinacja == 11 || Kombinacja == 16)
  {
    hcan.Init.Prescaler = 10;
    HAL_CAN_Stop(&hcan);
    if (HAL_CAN_Init(&hcan) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_CAN_Start(&hcan);
  }

  // Wysłanie aktualnej konfiguracji zegara w wiadomości CAN o ID 0x0
  tempTxHeader.StdId = 0x0;
  tempTxHeader.DLC = 1;
  tempTxHeader.IDE = CAN_ID_STD;
  tempTxHeader.RTR = CAN_RTR_DATA; 
  temp_TxData[0] = Kombinacja; 
  HAL_CAN_AddTxMessage(&hcan, &tempTxHeader, temp_TxData, &TxMailbox);
  HAL_Delay(1000); 
  
}  

void Przestaw_CAN(uint32_t CAN_Prescaler)
{
  hcan.Init.Prescaler = CAN_Prescaler;
  HAL_CAN_Stop(&hcan);
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_CAN_Start(&hcan);
  tempTxHeader.StdId = 0x1;
  tempTxHeader.DLC = 1; 
  tempTxHeader.IDE = CAN_ID_STD; 
  tempTxHeader.RTR = CAN_RTR_DATA; 
  temp_TxData[0] = CAN_Prescaler; // Wysyłanie aktualnego prescalera w pierwszym bajcie danych wiadomości CAN
  HAL_CAN_AddTxMessage(&hcan, &tempTxHeader, temp_TxData, &TxMailbox);
  HAL_Delay(1000);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);
  // Identyfikator wiadomości konfiguracyjnej: 0x0 , dane: 1 bajt określający konfigurację zegara
  if(RxHeader.StdId == 0x0)
  {
    Kombinacja = RxData[0];
    flaga_zmiany_zegara = 1;
  }
  else if (RxHeader.StdId == 0x1) 
  {
    CAN_Prescaler = RxData[0];
    flaga_zmiany_CAN = 1;
  }
  else if (RxHeader.StdId == 0x2) 
  {
    TxHeader.DLC = RxData[0];
    flaga_zmiany_DLC = 1;
  }
  else if (RxHeader.StdId == 0x3)
  {
    Delay = RxData[0];
  }
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
     printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
