/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_filex.c
  * @brief   FileX applicative file
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
#include "app_filex.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* Main thread stack size */
#define FX_APP_THREAD_STACK_SIZE  1024*2
/* Main thread priority */
#define FX_APP_THREAD_PRIO        10
/* USER CODE BEGIN PD */
/* Message content*/
typedef enum {
CARD_STATUS_CHANGED             = 99,
CARD_STATUS_DISCONNECTED        = 88,
CARD_STATUS_CONNECTED           = 77
} SD_ConnectionStateTypeDef;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MEDIA_CLOSED  1UL
#define MEDIA_OPENED  0UL
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
const UBaseType_t uxQueueLength = 5U;
/* Main thread global data structures. */
TaskHandle_t fx_app_thread;

/* Buffer for FileX FX_MEDIA sector cache. */
ALIGN_32BYTES (uint32_t fx_sd_media_memory[FX_STM32_SD_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)]);
/* Define FileX global data structures.  */
FX_MEDIA        sdio_disk;

/* USER CODE BEGIN PV */
static UINT media_status;
/* Define FileX global data structures. */
FX_FILE fx_file;
/* Define FreeRTOS global data structures. */
QueueHandle_t msg_queue;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* Main thread entry function. */
VOID fx_thread_entry(VOID *args);

/* USER CODE BEGIN PFP */
static uint8_t SD_IsDetected(uint32_t Instance);
static VOID media_close_callback (FX_MEDIA *media_ptr);
/* USER CODE END PFP */

/**
  * @brief  Application FileX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
*/
UINT MX_FileX_Init(VOID)
{
  UINT ret = FX_SUCCESS;

  /* USER CODE BEGIN MX_FileX_Init */

  /* Create the main thread. */
  if (xTaskCreate(fx_thread_entry, "fx thread entry", FX_APP_THREAD_STACK_SIZE,
                  (VOID *) NULL, FX_APP_THREAD_PRIO, &fx_app_thread) != pdPASS)
  {
    return FX_IO_ERROR;
  }

  msg_queue = xQueueCreate(uxQueueLength, sizeof(ULONG));
  if(msg_queue == NULL)
  {
    Error_Handler();
  }
  /* USER CODE END MX_FileX_Init */

  /* Initialize FileX. */
  fx_system_initialize();

  /* USER CODE BEGIN MX_FileX_Init 1 */

  /* USER CODE END MX_FileX_Init 1 */

  return ret;
}

/**
  * @brief  Main thread entry.
  * @param thread_input: ULONG user argument used by the thread entry
  * @retval none
  */
 VOID fx_thread_entry(VOID *input)
 {

  UINT sd_status = FX_SUCCESS;

  /* USER CODE BEGIN fx_thread_entry 0 */
  ULONG r_msg;
  ULONG s_msg = CARD_STATUS_CHANGED;
  ULONG last_status = CARD_STATUS_DISCONNECTED;
  ULONG bytes_read;
  CHAR read_buffer[32];
  CHAR data[] = "This is FileX working on STM32";
 /* USER CODE END fx_thread_entry 0 */

  /* Open the SD disk driver */
  sd_status =  fx_media_open(&sdio_disk, FX_SD_VOLUME_NAME, fx_stm32_sd_driver, (VOID *)FX_NULL, (VOID *) fx_sd_media_memory, sizeof(fx_sd_media_memory));

  /* Check the media open sd_status */
  if (sd_status != FX_SUCCESS)
  {
     /* USER CODE BEGIN SD DRIVER get info error */
     Error_Handler();
    /* USER CODE END SD DRIVER get info error */
  }

  /* USER CODE BEGIN fx_thread_entry 1 */
  fx_media_close_notify_set(&sdio_disk, media_close_callback);
  if(SD_IsDetected(FX_STM32_SD_INSTANCE) == HAL_OK)
  {
    /* SD card is already inserted, place the info into the queue */
    xQueueSend(msg_queue, &s_msg, NULL);
  }
  else
  {
    /* Indicate that SD card is not inserted from start */
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
  }

  /* Infinite Loop */
  for( ;; )
  {

    /* We wait here for a valid SD card insertion event, if it is not inserted already */
    while(1)
    {

      while(xQueueReceive(msg_queue, &r_msg, pdMS_TO_TICKS(500)) != pdPASS)
      {
        /* Toggle GREEN LED to indicate idle state after a successful operation */
        if(last_status == CARD_STATUS_CONNECTED)
        {
          HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        }
      }

      /* check if we received the correct event message */
      if(r_msg == CARD_STATUS_CHANGED)
      {
        /* reset the sd_status */
        r_msg = 0;

        /* for debouncing purpose we wait a bit till it settles down */
        vTaskDelay(pdMS_TO_TICKS(500));

        if(SD_IsDetected(FX_STM32_SD_INSTANCE) == HAL_OK)
        {
          /* We have a valid SD insertion event, start processing.. */
          /* Update last known sd_status */
          last_status = CARD_STATUS_CONNECTED;
          HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); /*Led Red Off*/
          break;
        }
        else
        {
          /* Update last known sd_status */
          last_status = CARD_STATUS_DISCONNECTED;
          HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);  /*LED Green Off*/
          HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); /*LED Red On*/
        }
      }

    }

    /* Create a file called STM32.TXT in the root directory. */
    if (media_status == MEDIA_CLOSED)
    {
      sd_status = fx_media_open(&sdio_disk, FX_SD_VOLUME_NAME, fx_stm32_sd_driver, (VOID *)FX_NULL, (VOID *) fx_sd_media_memory, sizeof(fx_sd_media_memory));
      /* Check the media open sd_status */
      if (sd_status != FX_SUCCESS)
      {
        /* USER CODE BEGIN SD DRIVER get info error */
        Error_Handler();
        /* USER CODE END SD DRIVER get info error */
      }
      media_status = MEDIA_OPENED;
    }

    sd_status =  fx_file_create(&sdio_disk, "STM32.TXT");

    /* Check the create sd_status. */
    if (sd_status != FX_SUCCESS)
    {
      /* Check for an already created sd_status. This is expected on the
      second pass of this loop! */
      if (sd_status != FX_ALREADY_CREATED)
      {
        /* Create error, call error handler.  */
        Error_Handler();
      }
    }

    /* Open the test file. */
    sd_status =  fx_file_open(&sdio_disk, &fx_file, "STM32.TXT", FX_OPEN_FOR_WRITE);

    /* Check the file open sd_status. */
    if (sd_status != FX_SUCCESS)
    {
      /* Error opening file, call error handler. */
      Error_Handler();
    }

    /* Seek to the beginning of the test file. */
    sd_status =  fx_file_seek(&fx_file, 0);

    /* Check the file seek sd_status. */
    if (sd_status != FX_SUCCESS)
    {
      /* Error performing file seek, call error handler. */
      Error_Handler();
    }

    /* Write a string to the test file. */
    sd_status =  fx_file_write(&fx_file, data, sizeof(data));

    /* Check the file write sd_status. */
    if (sd_status != FX_SUCCESS)
    {
      /* Error writing to a file, call error handler. */
      Error_Handler();
    }

    /* Close the test file. */
    sd_status =  fx_file_close(&fx_file);

    /* Check the file close sd_status. */
    if (sd_status != FX_SUCCESS)
    {
      /* Error closing the file, call error handler. */
      Error_Handler();
    }

    sd_status = fx_media_flush(&sdio_disk);

    /* Check the media flush  sd_status. */
    if (sd_status != FX_SUCCESS)
    {
      /* Error closing the file, call error handler. */
      Error_Handler();
    }

    /* Open the test file.  */
    sd_status =  fx_file_open(&sdio_disk, &fx_file, "STM32.TXT", FX_OPEN_FOR_READ);

    /* Check the file open sd_status. */
    if (sd_status != FX_SUCCESS)
    {
      /* Error opening file, call error handler. */
      Error_Handler();
    }

    /* Seek to the beginning of the test file. */
    sd_status =  fx_file_seek(&fx_file, 0);

    /* Check the file seek sd_status. */
    if (sd_status != FX_SUCCESS)
    {
      /* Error performing file seek, call error handler. */
      Error_Handler();
    }

    /* Read the first 28 bytes of the test file. */
    sd_status =  fx_file_read(&fx_file, read_buffer, sizeof(data), &bytes_read);

    /* Check the file read sd_status. */
    if ((sd_status != FX_SUCCESS) || (bytes_read != sizeof(data)))
    {
      /* Error reading file, call error handler. */
      Error_Handler();
    }

    /* Close the test file. */
    sd_status =  fx_file_close(&fx_file);

    /* Check the file close sd_status.  */
    if (sd_status != FX_SUCCESS)
    {
      /* Error closing the file, call error handler. */
      Error_Handler();
    }

    /* Close the media. */
    sd_status =  fx_media_close(&sdio_disk);

    /* Check the media close sd_status. */
    if (sd_status != FX_SUCCESS)
    {
      /* Error closing the media, call error handler. */
      Error_Handler();
    }

  }
  /* USER CODE END fx_thread_entry 1 */
  }

/* USER CODE BEGIN 1 */

/**
 * @brief  Detects if SD card is correctly plugged in the memory slot or not.
 * @param Instance  SD Instance
 * @retval Returns if SD is detected or not
 */
static uint8_t SD_IsDetected(uint32_t Instance)
{
  uint8_t ret;
  if(Instance >= 1)
  {
    ret = HAL_ERROR;
  }
  else
  {
    /* Check SD card detect pin */
    if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_Port, SD_DETECT_Pin) == GPIO_PIN_SET)
    {
      ret = HAL_ERROR;
    }
    else
    {
      ret = HAL_OK;
    }
  }

  return ret;
}

/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  ULONG s_msg = CARD_STATUS_CHANGED;

  if(GPIO_Pin == SD_DETECT_Pin)
  {
    xQueueSendFromISR(msg_queue, &s_msg, NULL);
  }
}

/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  ULONG s_msg = CARD_STATUS_CHANGED;

  if(GPIO_Pin == SD_DETECT_Pin)
  {
    xQueueSendFromISR(msg_queue, &s_msg, NULL);
  }
}

/**
  * @brief  Media close notify callback function.
  * @param  media_ptr: Media control block pointer
  * @retval None
  */
static VOID media_close_callback(FX_MEDIA *media_ptr)
{
  media_status = MEDIA_CLOSED;
}

/* USER CODE END 1 */
