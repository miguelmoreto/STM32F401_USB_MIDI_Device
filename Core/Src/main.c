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
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_midi_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MIDI_CHANNEL		00
#define MIDI_CABLE			00
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* Flags: */
uint8_t flag_tim11 = 0;
uint8_t midi_nt_received_flag = 1;	// MIDI note message received flag.
uint8_t midi_pc_received_flag = 1;	// MIDI program change message received flag.
uint8_t midi_cc_received_flag = 1;	// MIDI control change message received flag.

/* MIDI unions: */
MIDI_NoteUnion_typedef midi_note_tx;		// MIDI note message variable (transmit)
MIDI_NoteUnion_typedef midi_note_rx;		// MIDI note message variable (received)
MIDI_ProgChangeUnion_typedef midi_pc_tx;	// MIDI program change message variable (transmit)
MIDI_ProgChangeUnion_typedef midi_pc_rx;	// MIDI program change message variable (received)
MIDI_CtrChangeUnion_typedef midi_cc_tx;		// MIDI control change message variable (transmit)
MIDI_CtrChangeUnion_typedef midi_cc_rx;		// MIDI control change message variable (received)

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
	// MIDI Note message data initialization:
	midi_note_tx.fields.cable = MIDI_CABLE;
	//midi_note.fields.CIN = 0x09; // MIDI note on. This atribution is optional.
	midi_note_tx.fields.channel = MIDI_CHANNEL;
	midi_note_tx.fields.type = MIDI_NOTE_ON;
	midi_note_tx.fields.note = 0x26;
	midi_note_tx.fields.velocity = 0x7F; // 127

	// MIDI Program change message data initialization:
	midi_pc_tx.fields.cable = MIDI_CABLE;
	midi_pc_tx.fields.CIN = 0x0C;  // This atribution is optional.
	midi_pc_tx.fields.channel = MIDI_CHANNEL;
	midi_pc_tx.fields.type = 0x0C; // This atribution is optional.
	midi_pc_tx.fields.program = 2;
	midi_pc_tx.fields.unused = 0;  // This atribution is optional.

	// MIDI Control change message data initialization:
	midi_cc_tx.fields.cable = MIDI_CABLE;
	midi_cc_tx.fields.channel = MIDI_CHANNEL;
	midi_cc_tx.fields.controller = 0x2A;
	midi_cc_tx.fields.value = 0x7F; // Valeu 127: ON state

	uint8_t temp1 = 0;
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
  MX_TIM11_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
	/* Starting timers: */
	HAL_TIM_Base_Start_IT(&htim11); // TIM11 used for counting half seconds (blink led)
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		if(flag_tim11 == 1){ // Half second has passed.
			// Led Blink:
			HAL_GPIO_TogglePin(LED_ONBOARD_GPIO_Port, LED_ONBOARD_Pin);
			flag_tim11 = 0;
			if (temp1 == 1){
				midi_note_tx.fields.note = 0x26; // Snare (on Hydrogen MIDI in)
				temp1 = 0;
			}else{
				midi_note_tx.fields.note = 0x24; // Kick drum (on Hydrogen MIDI in)
				temp1 = 1;
			}
			MIDI_SendNote(&midi_note_tx); // To send a MIDI note message
			//MIDI_SendProgramChange(&midi_pc); // To send a MIDI Program Change message
			//midi_pc.fields.program = midi_pc.fields.program + 10; // just for testing
			//MIDI_SendControlChange(&midi_cc); // To send a MIDI Controller Change message
			//midi_cc.fields.value = midi_cc.fields.value + 10; // just for testing
		} // flag_tim11 == 1

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

/* This callback is executed when the USB MIDI Class receives a MIDI note message. */
void MIDI_ReceivedNoteCallback(MIDI_NoteUnion_typedef receivedNote){
	/* Doing stuff in main loop. */
	memcpy(midi_note_rx.array, receivedNote.array, 4);
	/* TODO: declare global midi received variables in usbd_midi_if.c */
	midi_nt_received_flag = 1;

}

/* This callback is executed when the USB MIDI Class receives a MIDI Program Change message. */
void MIDI_ReceivedProgramChangeCallback(MIDI_ProgChangeUnion_typedef receivedProgramChange){
	/* Doing stuff in main loop. */
	memcpy(midi_pc_rx.array, receivedProgramChange.array, 4);
	/* TODO: declare global midi received variables in usbd_midi_if.c */
	midi_pc_received_flag = 1;

}

/* This callback is executed when the USB MIDI Class receives a MIDI Contro Change message. */
void MIDI_ReceivedControlChangeCallback(MIDI_CtrChangeUnion_typedef receivedControlChange){
	/* Doing stuff in main loop. */
	memcpy(midi_cc_rx.array, receivedControlChange.array, 4);
	/* TODO: declare global midi received variables in usbd_midi_if.c */
	midi_cc_received_flag = 1;
}


/* Timer interrupt callback */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == TIM11){
		flag_tim11 = 1;
	} // htim->Instance == TIM11
} // HAL_TIM_PeriodElapsedCallback

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
