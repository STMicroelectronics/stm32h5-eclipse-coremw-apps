/**
  ******************************************************************************
  * @file    ST1633i.h
  * @author  MCD Application Team
  * @brief   This file contains all the constants parameters for the ST1633i-CTG
  *          LCD component.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ST1633i_H
#define __ST1633i_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup ST1633i
  * @{
  */

/** @defgroup ST1633i_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup ST1633i_Exported_Constants
  * @{
  */

/**
  * @brief  ST1633i Size
  */
#define  ST1633i_WIDTH    ((uint16_t)480)          /* LCD PIXEL WIDTH            */
#define  ST1633i_HEIGHT   ((uint16_t)272)          /* LCD PIXEL HEIGHT           */

/**
  * @brief  ST1633i Timing
  */
#define  ST1633i_HSYNC            ((uint16_t)4)   /* Horizontal synchronization */
#define  ST1633i_HBP              ((uint16_t)43)   /* Horizontal back porch      */
#define  ST1633i_HFP              ((uint16_t)8)   /* Horizontal front porch     */
#define  ST1633i_VSYNC            ((uint16_t)4)   /* Vertical synchronization   */
#define  ST1633i_VBP              ((uint16_t)12)   /* Vertical back porch        */
#define  ST1633i_VFP              ((uint16_t)8)   /* Vertical front porch       */


/**
  * @}
  */

/** @defgroup ST1633i_Exported_Functions
  * @{
  */

/**
  * @}
  */

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

#endif /* __ST1633i_H */
