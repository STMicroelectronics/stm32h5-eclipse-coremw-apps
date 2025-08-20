/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_usbx_device.c
  * @brief   USBX Device applicative file
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
#include "app_usbx_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
static __ALIGNED(4) UCHAR ux_device_memory_buffer[USBX_DEVICE_MEMORY_STACK_SIZE];

static ULONG hid_mouse_interface_number;
static ULONG hid_mouse_configuration_number;
static UX_SLAVE_CLASS_HID_PARAMETER hid_mouse_parameter;
TaskHandle_t ux_device_app_thread;
TaskHandle_t app_ux_device_thread;
TaskHandle_t usbd_hid_mouse_thread;
VOID USBX_APP_Device_Init(VOID);
/* USER CODE BEGIN PV */
TaskHandle_t ux_hid_thread;
extern uint8_t User_Button_State;


#define APP_DEFAULT_STACK_SIZE    (configMINIMAL_STACK_SIZE)
#define APP_DEFAULT_PRIORITY      (configMAX_PRIORITIES / 2U)

QueueHandle_t                        ux_app_MsgQueue;

extern PCD_HandleTypeDef      hpcd_USB_DRD_FS;
#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
__ALIGN_BEGIN USB_MODE_STATE  USB_Device_State_Msg __ALIGN_END;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
 static VOID app_ux_device_thread_entry(VOID *args);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application USBX Device Initialization.
  * @param  memory_ptr: memory pointer
  * @retval status
  */
UINT MX_USBX_Device_Init(VOID)
{
  UINT ret = UX_SUCCESS;
  UCHAR *device_framework_high_speed;
  UCHAR *device_framework_full_speed;
  ULONG device_framework_hs_length;
  ULONG device_framework_fs_length;
  ULONG string_framework_length;
  ULONG language_id_framework_length;
  UCHAR *string_framework;
  UCHAR *language_id_framework;
  UCHAR *pointer;

  pointer = ux_device_memory_buffer;
  /* Initialize USBX Memory */
  if (ux_system_initialize(pointer, USBX_DEVICE_MEMORY_STACK_SIZE, UX_NULL, 0) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_SYSTEM_INITIALIZE_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_SYSTEM_INITIALIZE_ERROR */
  }

  /* Get Device Framework High Speed and get the length */
  device_framework_high_speed = USBD_Get_Device_Framework_Speed(USBD_HIGH_SPEED,
                                                                &device_framework_hs_length);

  /* Get Device Framework Full Speed and get the length */
  device_framework_full_speed = USBD_Get_Device_Framework_Speed(USBD_FULL_SPEED,
                                                                &device_framework_fs_length);

  /* Get String Framework and get the length */
  string_framework = USBD_Get_String_Framework(&string_framework_length);

  /* Get Language Id Framework and get the length */
  language_id_framework = USBD_Get_Language_Id_Framework(&language_id_framework_length);

  /* Install the device portion of USBX */
  if (ux_device_stack_initialize(device_framework_high_speed,
                                 device_framework_hs_length,
                                 device_framework_full_speed,
                                 device_framework_fs_length,
                                 string_framework,
                                 string_framework_length,
                                 language_id_framework,
                                 language_id_framework_length,
                                 UX_NULL) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_DEVICE_INITIALIZE_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_DEVICE_INITIALIZE_ERROR */
  }

  /* Initialize the hid mouse class parameters for the device */
  hid_mouse_parameter.ux_slave_class_hid_instance_activate         = USBD_HID_Mouse_Activate;
  hid_mouse_parameter.ux_slave_class_hid_instance_deactivate       = USBD_HID_Mouse_Deactivate;
  hid_mouse_parameter.ux_device_class_hid_parameter_report_address = USBD_HID_ReportDesc(INTERFACE_HID_MOUSE);
  hid_mouse_parameter.ux_device_class_hid_parameter_report_length  = USBD_HID_ReportDesc_length(INTERFACE_HID_MOUSE);
  hid_mouse_parameter.ux_device_class_hid_parameter_report_id      = UX_FALSE;
  hid_mouse_parameter.ux_device_class_hid_parameter_callback       = USBD_HID_Mouse_SetReport;
  hid_mouse_parameter.ux_device_class_hid_parameter_get_callback   = USBD_HID_Mouse_GetReport;

  /* USER CODE BEGIN HID_MOUSE_PARAMETER */

  /* USER CODE END HID_MOUSE_PARAMETER */

  /* Get hid mouse configuration number */
  hid_mouse_configuration_number = USBD_Get_Configuration_Number(CLASS_TYPE_HID, INTERFACE_HID_MOUSE);

  /* Find hid mouse interface number */
  hid_mouse_interface_number = USBD_Get_Interface_Number(CLASS_TYPE_HID, INTERFACE_HID_MOUSE);

  /* Initialize the device hid Mouse class */
  if (ux_device_stack_class_register(_ux_system_slave_class_hid_name,
                                     ux_device_class_hid_entry,
                                     hid_mouse_configuration_number,
                                     hid_mouse_interface_number,
                                     &hid_mouse_parameter) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_DEVICE_HID_MOUSE_REGISTER_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_DEVICE_HID_MOUSE_REGISTER_ERROR */
  }

  /* Create app_ux_device_thread.  */
   if (xTaskCreate(app_ux_device_thread_entry, "app ux device thread entry", APP_DEFAULT_STACK_SIZE,
                       (VOID *) NULL, APP_DEFAULT_PRIORITY, &app_ux_device_thread) != pdPASS)
  {
    return UX_ERROR;
  }

  /* Create usbd_hid_mouse_thread_entry.  */
  if ( xTaskCreate(usbx_hid_thread_entry, "usbd hid mouse thread entry", APP_DEFAULT_STACK_SIZE,
                   (VOID *) NULL, APP_DEFAULT_PRIORITY, &usbd_hid_mouse_thread) != pdPASS)
  {
    return UX_ERROR;
  }

  ux_app_MsgQueue = xQueueCreate(1,sizeof(ULONG));

  return ret;
}

/**
  * @brief  Function implementing app_ux_device_thread_entry.
  * @param  thread_input: User thread input parameter.
  * @retval none
  */

VOID app_ux_device_thread_entry(VOID *input)
{
  /* USER CODE BEGIN app_ux_device_thread_entry */

  /* Initialization of USB device */
  USBX_APP_Device_Init();

  /* Wait for message queue to start/stop the device */
  while(1)
  {
    /* Wait for a device to be connected */
    if (xQueueReceive(ux_app_MsgQueue, &USB_Device_State_Msg,
                         portMAX_DELAY)!= pdPASS)
    {
      /*Error*/
      Error_Handler();
    }
    /* Check if received message equal to USB_PCD_START */
    if (USB_Device_State_Msg == START_USB_DEVICE)
    {
      /* Start device USB */
      HAL_PCD_Start(&hpcd_USB_DRD_FS);
    }
    /* Check if received message equal to USB_PCD_STOP */
    else if (USB_Device_State_Msg == STOP_USB_DEVICE)
    {
      /* Stop device USB */
      HAL_PCD_Stop(&hpcd_USB_DRD_FS);
    }
    /* Else Error */
    else
    {
      /*Error*/
      Error_Handler();
    }
    vTaskDelay(MS_TO_TICK(10));
  }
  /* USER CODE END app_ux_device_thread_entry */
}

/* USER CODE BEGIN 1 */

/**
  * @brief  USBX_APP_Device_Init
  *         Initialization of USB device.
  * @param  none
  * @retval none
  */
VOID USBX_APP_Device_Init(VOID)
{
  /* USER CODE BEGIN USB_Device_Init_PreTreatment_0 */
  /* USER CODE END USB_Device_Init_PreTreatment_0 */

  /* USB_DRD_FS init function */
  MX_USB_PCD_Init();

  /* USER CODE BEGIN USB_Device_Init_PreTreatment_1 */
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x00, PCD_SNG_BUF, 0x0C);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x80, PCD_SNG_BUF, 0x4C);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, USBD_HID_MOUSE_EPIN_ADDR, PCD_SNG_BUF, 0x8C);
  /* USER CODE END USB_Device_Init_PreTreatment_1 */

  /* initialize the device controller driver*/
  ux_dcd_stm32_initialize((ULONG)USB_DRD_FS, (ULONG)&hpcd_USB_DRD_FS);

  /* USER CODE BEGIN USB_Device_Init_PostTreatment */

  /* USER CODE END USB_Device_Init_PostTreatment */
}

/**
  * @brief  HAL_GPIO_EXTI_Rising_Callback
  *         EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{

  /* Check if EXTI from User Button */
  if (GPIO_Pin == GPIO_PIN_13)
  {
    User_Button_State ^= 1U;
  }
}

/**
  * @brief  USB utility interrupt disable.
  * @retval none
  */
ALIGN_TYPE _ux_utility_interrupt_disable(VOID)
{
  UINT interrupt_save;

  interrupt_save = __get_PRIMASK();
  __disable_irq();

  return interrupt_save;
}

/**
  * @brief  USB utility interrupt restore.
  * @retval none
  */
VOID _ux_utility_interrupt_restore(ALIGN_TYPE flags)
{
  __set_PRIMASK(flags);
}

/**
  * @brief  Get Time Tick for host timing.
  * @retval time tick
  */
ULONG _ux_utility_time_get(VOID)
{
  return HAL_GetTick();
}

/* USER CODE END 1 */
