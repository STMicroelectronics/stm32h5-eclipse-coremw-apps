/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_os_port_mx.h
  * @brief   This file contains the core os portability macro definition.
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
#ifndef USBPD_CORE_OSPORT_H_
#define USBPD_CORE_OSPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ux_port.h"

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_CORE_OS
  * @{
  */

/** @addtogroup USBPD_CORE_OS_Macro
  * @{
  */
/* Exported define -----------------------------------------------------------*/
/**
  * @brief macro definition used to define the task function
  */
#define DEF_TASK_FUNCTION(__FUNCTION__)   void (__FUNCTION__)(void *args)

/**
  * @brief macro definition used to initialize the OS environment
  */
#define OS_INIT()          \
  uint32_t _retr = pdPASS;


/**
  * @brief macro definition the define a queue type
  */
#define OS_QUEUE_ID QueueHandle_t

/**
  * @brief macro definition the define a queue type
  */
#define OS_ELEMENT_SIZE 1

/**
  * @brief macro definition used to define a queue
  */
#define OS_CREATE_QUEUE(_ID_,_ELT_,_ELTSIZE_)                            \
  do{                                                                    \
    _ID_ = xQueueCreate((_ELTSIZE_), (_ELT_)*sizeof(ULONG)*(_ELTSIZE_)); \
    if(_ID_ == NULL)                                                     \
    {                                                                    \
      goto error;                                                        \
    }                                                                    \
  } while(0);

/**
  * @brief macro definition used to read a queue message
  */
#define OS_GETMESSAGE_QUEUE(_ID_, _TIME_)                                \
  do {                                                                   \
    ULONG value;                                                         \
    xQueueReceive((_ID_), (void*)&value, (_TIME_));                      \
  } while(0)

/**
  * @brief macro definition used to define put a message inside the queue
  */
#define OS_PUT_MESSAGE_QUEUE(_ID_,_MSG_,_TIMEOUT_) \
  do{                                                                    \
    ULONG _msg = _MSG_;                                                  \
    xQueueSend(_ID_, &_msg,(_TIMEOUT_));                                 \
  }while(0)

/**
  * @brief macro definition of the TASK id
  */
#define OS_TASK_ID   TaskHandle_t

/**
  * @brief macro definition used to create a task
  */
#define OS_CREATE_TASK(_ID_,_NAME_,_STACK_SIZE_,_PRIORITY_, _TASK_HANDLE_) \
  do {                                                                     \
    _retr = xTaskCreate(_ID_,#_NAME_, (_STACK_SIZE_),                      \
                         NULL, _PRIORITY_,                                 \
                         &(_TASK_HANDLE_));                                \
    if(_retr != pdPASS)                                                    \
    {                                                                      \
      goto error;                                                          \
    }                                                                      \
  } while(0);

/**
  * @brief macro definition used to check is task is suspended
  */
#define OS_TASK_IS_SUPENDED(_ID_) (eSuspended == eTaskGetState(_ID_))

/**
  * @brief macro definition used to get the task ID
  */
#define OS_TASK_GETID()          xTaskGetCurrentTaskHandle()

/**
  * @brief macro definition used to suspend a task
  */
#define OS_TASK_SUSPEND(_ID_)    vTaskSuspend(_ID_)

/**
  * @brief macro definition used to resume a task
  */
#define OS_TASK_RESUME(_ID_)     vTaskResume(_ID_)

/**
  * @brief macro definition used to manage the delay
  */
#define OS_DELAY(_TIME_)         vTaskDelay(_TIME_)

/**
  * @brief macro definition used to start the task scheduling
  */
#define OS_KERNEL_START() /* This function is not managed at usbpd level in the case of freertos */

/* Exported types ------------------------------------------------------------*/
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif
#endif /* USBPD_CORE_OSPORT_H_ */

