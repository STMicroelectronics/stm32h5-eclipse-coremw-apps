/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_usbx_host.c
  * @brief   USBX host applicative file
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
#include "app_usbx_host.h"
#include "stm32h573i_discovery_usbpd_pwr.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define APP_DEFAULT_PRIORITY       (configMAX_PRIORITIES / 2U)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TaskHandle_t ux_host_app_thread_handle;
/* USER CODE BEGIN PV */
TaskHandle_t hid_mouse_thread_handle;
TaskHandle_t hid_keyboard_thread_handle;

QueueHandle_t               ux_app_MsgQueue_UCPD;
UX_HOST_CLASS_HID          *hid_instance;
UX_HOST_CLASS_HID_MOUSE    *mouse;
UX_HOST_CLASS_HID_KEYBOARD *keyboard;

static __ALIGNED(4) UCHAR ux_host_memory_buffer[USBX_HOST_MEMORY_STACK_SIZE];

#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
__ALIGN_BEGIN USB_MODE_STATE                            USB_Host_State_Msg   __ALIGN_END;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID app_ux_host_thread_entry(void *args);
static UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS *current_class, VOID *current_instance);
static VOID ux_host_error_callback(UINT system_level, UINT system_context, UINT error_code);
/* USER CODE BEGIN PFP */
extern HCD_HandleTypeDef hhcd_USB_DRD_FS;
/* USER CODE END PFP */

/**
  * @brief  Application USBX Host Initialization.
  * @param  memory_ptr: memory pointer
  * @retval status
  */
UINT MX_USBX_Host_Init(VOID)
{
  UINT ret = UX_SUCCESS;
  UCHAR *pointer;

  UINT status = UX_SUCCESS;

  pointer = ux_host_memory_buffer;

  /* Initialize USBX Memory */
  if (ux_system_initialize(pointer, USBX_HOST_MEMORY_STACK_SIZE, UX_NULL, 0) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_SYSTEM_INITIALIZE_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_SYSTEM_INITIALIZE_ERROR */
  }

  /* Install the host portion of USBX */
  if (ux_host_stack_initialize(ux_host_event_callback) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_HOST_INITIALIZE_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_HOST_INITIALIZE_ERROR */
  }

  /* Register a callback error function */
  ux_utility_error_callback_register(&ux_host_error_callback);

  /* Initialize the host hid class */
  if (ux_host_stack_class_register(_ux_system_host_class_hid_name,
                                   ux_host_class_hid_entry) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_HSOT_HID_REGISTER_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_HSOT_HID_REGISTER_ERROR */
  }

  /* Initialize the host hid mouse client */
  if (ux_host_class_hid_client_register(_ux_system_host_class_hid_client_mouse_name,
                                        ux_host_class_hid_mouse_entry) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_HOST_HID_MOUSE_REGISTER_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_HOST_HID_MOUSE_REGISTER_ERROR */
  }

  /* Initialize the host hid keyboard client */
  if (ux_host_class_hid_client_register(_ux_system_host_class_hid_client_keyboard_name,
                                        ux_host_class_hid_keyboard_entry) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_HOST_HID_KEYBOARD_REGISTER_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_HOST_HID_KEYBOARD_REGISTER_ERROR */
  }

  /* Create the host thread entry */
  status = xTaskCreate(app_ux_host_thread_entry, "host thread entry",
                       UX_HOST_APP_THREAD_STACK_SIZE, NULL,
                       APP_DEFAULT_PRIORITY, &ux_host_app_thread_handle);
  if (status != pdPASS)
  {
    return UX_ERROR;
  }

  /* Create the hid keyboard thread */
  status = xTaskCreate(hid_keyboard_thread_entry, "hid keyboard thread entry",
                       UX_HOST_APP_THREAD_STACK_SIZE, NULL,
                       APP_DEFAULT_PRIORITY, &hid_keyboard_thread_handle);
  if (status != pdPASS)
  {
    return UX_ERROR;
  }

  /* Create the hid mouse thread */
  status = xTaskCreate(hid_mouse_thread_entry, "hid mouse thread entry",
                       UX_HOST_APP_THREAD_STACK_SIZE, NULL,
                       APP_DEFAULT_PRIORITY, &hid_mouse_thread_handle);
  if (status != pdPASS)
  {
    return UX_ERROR;
  }

  /* Create the ux_app_MsgQueue_UCPD queue message */
  ux_app_MsgQueue_UCPD = xQueueCreate(1,sizeof(ULONG));

  if (ux_app_MsgQueue_UCPD == NULL)
  {
    return UX_ERROR;
  }

  /* USER CODE END MX_USBX_Host_Init1 */

  return ret;
}

/**
  * @brief  Function implementing app_ux_host_thread_entry.
  * @param  thread_input: User thread input parameter.
  * @retval none
  */
static VOID app_ux_host_thread_entry(void *args)
{
  /* USER CODE BEGIN app_ux_host_thread_entry */

  /* Initialization of USB host */
  USBX_APP_Host_Init();

  while (1)
  {
    /* wait for message queue from callback event */
    if(xQueueReceive(ux_app_MsgQueue_UCPD, &USB_Host_State_Msg, portMAX_DELAY)!= pdPASS)
    {
     Error_Handler();
    }
    /* Check if received message equal to START_USB_HOST */
    if (USB_Host_State_Msg == START_USB_HOST)
    {
      /* Start USB Host */
      HAL_HCD_Start(&hhcd_USB_DRD_FS);
    }
    /* Check if received message equal to STOP_USB_HOST */
    else if (USB_Host_State_Msg == STOP_USB_HOST)
    {
      /* Stop USB Host */
      HAL_HCD_Stop(&hhcd_USB_DRD_FS);
    }
    /* Else Error */
    else
    {
      /*Error*/
      Error_Handler();
    }
   vTaskDelay(MS_TO_TICK(10));
  }
  /* USER CODE END app_ux_host_thread_entry */
}

/**
  * @brief  ux_host_event_callback
  *         This callback is invoked to notify application of instance changes.
  * @param  event: event code.
  * @param  current_class: Pointer to class.
  * @param  current_instance: Pointer to class instance.
  * @retval status
  */
UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS *current_class, VOID *current_instance)
{
  UINT status = UX_SUCCESS;

  /* USER CODE BEGIN ux_host_event_callback0 */
  /* Get current Hid Client */
  UX_HOST_CLASS_HID_CLIENT *client  = (UX_HOST_CLASS_HID_CLIENT *)current_instance;
  /* USER CODE END ux_host_event_callback0 */

  switch (event)
  {
    case UX_DEVICE_INSERTION:

      /* USER CODE BEGIN UX_DEVICE_INSERTION */

       /* Get current Hid Class */
      if (current_class -> ux_host_class_entry_function == ux_host_class_hid_entry)
      {
        if (hid_instance == UX_NULL)
        {
          /* Get current Hid Instance */
          hid_instance = (UX_HOST_CLASS_HID *)current_instance;
        }
      }

      /* USER CODE END UX_DEVICE_INSERTION */

      break;

    case UX_DEVICE_REMOVAL:

      /* USER CODE BEGIN UX_DEVICE_REMOVAL */

      /* Free HID Instance */
      if ((VOID*)hid_instance == current_instance)
      {
        hid_instance = UX_NULL;
      }

      /* USER CODE END UX_DEVICE_REMOVAL */

      break;

    case UX_HID_CLIENT_INSERTION:

      /* USER CODE BEGIN UX_HID_CLIENT_INSERTION */

      USBH_UsrLog("\nHID Client Plugged");

      /* Check the HID_client if this is a HID keyboard device */
      if (client -> ux_host_class_hid_client_handler == ux_host_class_hid_keyboard_entry)
      {
        /* Get current Hid Client */
        if (keyboard == UX_NULL)
        {
          keyboard = client -> ux_host_class_hid_client_local_instance;

          USBH_UsrLog("HID_Keyboard_Device");
          USBH_UsrLog("PID: %#x ", (UINT)keyboard ->ux_host_class_hid_keyboard_hid->ux_host_class_hid_device->ux_device_descriptor.idProduct);
          USBH_UsrLog("VID: %#x ", (UINT)keyboard ->ux_host_class_hid_keyboard_hid->ux_host_class_hid_device->ux_device_descriptor.idVendor);
          USBH_UsrLog("USB HID Host Keyboard App...");
          USBH_UsrLog("keyboard is ready...\n");
        }
      }

      /* Check the HID_client if this is a HID mouse device */
      if (client -> ux_host_class_hid_client_handler == ux_host_class_hid_mouse_entry)
      {
        /* Get current Hid Client */
        if (mouse == UX_NULL)
        {
          mouse = client -> ux_host_class_hid_client_local_instance;

          USBH_UsrLog("HID_Mouse_Device");
          USBH_UsrLog("PID: %#x ", (UINT)mouse ->ux_host_class_hid_mouse_hid->ux_host_class_hid_device->ux_device_descriptor.idProduct);
          USBH_UsrLog("VID: %#x ", (UINT)mouse ->ux_host_class_hid_mouse_hid->ux_host_class_hid_device->ux_device_descriptor.idVendor);
          USBH_UsrLog("USB HID Host Mouse App...");
          USBH_UsrLog("Mouse is ready...\n");
        }
      }
      /* USER CODE END UX_HID_CLIENT_INSERTION */

      break;

    case UX_HID_CLIENT_REMOVAL:

      /* USER CODE BEGIN UX_HID_CLIENT_REMOVAL */

      /* Clear hid client local instance */
      if ((VOID*)keyboard == client -> ux_host_class_hid_client_local_instance)
      {
        /* Clear hid keyboard instance */
        keyboard = UX_NULL;

        USBH_UsrLog("\nHID Client Keyboard Unplugged");
      }

      if ((VOID*)mouse == client -> ux_host_class_hid_client_local_instance)
      {
        /* Clear hid mouse instance */
        mouse = UX_NULL;

        USBH_UsrLog("\nHID Client Mouse Unplugged");
      }

      /* USER CODE END UX_HID_CLIENT_REMOVAL */

      break;

    case UX_DEVICE_CONNECTION:

      /* USER CODE BEGIN UX_DEVICE_CONNECTION */

      /* USER CODE END UX_DEVICE_CONNECTION */

      break;

    case UX_DEVICE_DISCONNECTION:

      /* USER CODE BEGIN UX_DEVICE_DISCONNECTION */

      /* USER CODE END UX_DEVICE_DISCONNECTION */

      break;

    default:

      /* USER CODE BEGIN EVENT_DEFAULT */

      /* USER CODE END EVENT_DEFAULT */

      break;
  }

  /* USER CODE BEGIN ux_host_event_callback1 */

  /* USER CODE END ux_host_event_callback1 */

  return status;
}

/**
  * @brief ux_host_error_callback
  *         This callback is invoked to notify application of error changes.
  * @param  system_level: system level parameter.
  * @param  system_context: system context code.
  * @param  error_code: error event code.
  * @retval Status
  */
VOID ux_host_error_callback(UINT system_level, UINT system_context, UINT error_code)
{
  /* USER CODE BEGIN ux_host_error_callback0 */

  /* USER CODE END ux_host_error_callback0 */

  switch (error_code)
  {
    case UX_DEVICE_ENUMERATION_FAILURE:

      /* USER CODE BEGIN UX_DEVICE_ENUMERATION_FAILURE */

      USBH_UsrLog("USB Device Enumeration Failure");

      /* USER CODE END UX_DEVICE_ENUMERATION_FAILURE */

      break;

    case  UX_NO_DEVICE_CONNECTED:

      /* USER CODE BEGIN UX_NO_DEVICE_CONNECTED */

      USBH_UsrLog("USB Device disconnected");
      /* USER CODE END UX_NO_DEVICE_CONNECTED */

      break;

    default:

      /* USER CODE BEGIN ERROR_DEFAULT */

      /* USER CODE END ERROR_DEFAULT */

      break;
  }

  /* USER CODE BEGIN ux_host_error_callback1 */

  /* USER CODE END ux_host_error_callback1 */
}

/* USER CODE BEGIN 1 */

/**
  * @brief  USBX_APP_Host_Init
  *         Initialization of USB host.
  * @param  none
  * @retval none
  */
VOID USBX_APP_Host_Init(VOID)
{
  /* USER CODE BEGIN USB_Host_Init_PreTreatment_0 */
  /* USER CODE END USB_Host_Init_PreTreatment_0 */

  /* Initialize the LL driver */
  MX_USB_HCD_Init();

  /* Register all the USB host controllers available in this system. */
  ux_host_stack_hcd_register(_ux_system_host_hcd_stm32_name,
                             _ux_hcd_stm32_initialize, (ULONG)USB_DRD_FS,
                             (ULONG)&hhcd_USB_DRD_FS);



  /* USER CODE BEGIN USB_Host_Init_PostTreatment1 */

  /* Start Application Message */
  USBH_UsrLog("**** USB DRD HID Host **** \n");
  USBH_UsrLog("USB Host library started.\n");

  /* Wait for Device to be attached */
  USBH_UsrLog("Starting HID Application");
  USBH_UsrLog("Connect your HID Device");

  /* USER CODE END USB_Host_Init_PostTreatment1 */
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
