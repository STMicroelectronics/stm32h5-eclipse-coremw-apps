/**
  ******************************************************************************
  * @file    usbpd_usb_if.c
  * @brief   This file contains the usb if functions.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbpd_usb_if.h"
#include "usbpd_trace.h"
#include "app_usbx_host.h"
/* USB include files ----------------------------------------------------------*/

/** @addtogroup STM32_USBPD_USBIF
  * @{
  */

/* Private enums -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/
extern HCD_HandleTypeDef hhcd_USB_DRD_FS;
/**
  * @brief  Initialize billboard driver
  * @param  None
  * @retval status
  */
int32_t USBPD_USBIF_Init(void)
{
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, 0, 0, (uint8_t *)"USB_IF init", 11);
  return 0;
}


/** @addtogroup USBPD_CORE_USBIF_Exported_Functions
  * @{
  */
void USBPD_USBIF_DeviceStart(uint32_t PortNum)
{
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *)"USBIF Device start", 18);
}

void USBPD_USBIF_DeviceStop(uint32_t PortNum)
{
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *)"USBIF Device stop", 16);
}

void USBPD_USBIF_HostStart(uint32_t PortNum)
{
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *)"USBIF host start", 16);
  HAL_HCD_Start(&hhcd_USB_DRD_FS);
}

void USBPD_USBIF_HostStop(uint32_t PortNum)
{
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *)"USBIF host stop", 14);
  HAL_HCD_Stop(&hhcd_USB_DRD_FS);
}

void USBPD_USBIF_DeviceBillboard(uint32_t PortNum)
{
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *)"USBIF device billboard", 22);
}

void USBPD_USBIF_Swap2Host(uint32_t PortNum)
{
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *)"USBIF swap to host", 18);
}

void USBPD_USBIF_Swap2Device(uint32_t PortNum)
{
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *)"USBIF swap to device", 20);
}

void USBPD_USBIF_DeviceSetBOSInfo(uint32_t PortNum, void *DataPtr)
{
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *)"USBIF set BOS info", 18);
}

void USBPD_USBIF_DeviceSetVDMInfo(uint32_t PortNum, void *DataPtr)
{
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *)"USBIF set VDM info", 18);
}

/**
  * @}
  */

/** @addtogroup USBPD_CORE_USBIF_Private_Functions
  * @{
  */

/**
  * @}
  */
