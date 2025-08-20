/***************************************************************************
  * Copyright (c) 2024 Microsoft Corporation
  * Copyright (c) 2024 STMicroelectronics
  *
  * This program and the accompanying materials are made available under the
  * terms of the MIT License which is available at
  * https://opensource.org/licenses/MIT.
  *
  * SPDX-License-Identifier: MIT
  **************************************************************************/

#ifndef LX_STM32_OSPI_DRIVER_H
#define LX_STM32_OSPI_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "lx_api.h"
#include "stm32h5xx_hal.h"
#include "mx25lm51245g.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* The following semaphore is being to notify about RX/TX completion. It needs to be released in the transfer callbacks */
extern SemaphoreHandle_t xspi_rx_semaphore;
extern SemaphoreHandle_t xspi_tx_semaphore;

/* Exported constants --------------------------------------------------------*/

/* the OctoSPI instance, default value set to 0 */
#define LX_STM32_OSPI_INSTANCE                           0

/* define an offset from which to start accessing the NOR Flash memory.
 * It must be a multiple of LX_STM32_OSPI_SECTOR_SIZE, default value set to 0.
 */
#define LX_STM32_OSPI_BASE_ADDRESS                       0

#define LX_STM32_OSPI_DEFAULT_TIMEOUT                    10 * configTICK_RATE_HZ

#define LX_STM32_DEFAULT_SECTOR_SIZE                     LX_STM32_OSPI_SECTOR_SIZE
#define LX_STM32_OSPI_DMA_API                            1

/* when set to 1 LevelX is initializing the OctoSPI memory,
 * otherwise it is the up to the application to perform it.
 */
#define LX_STM32_OSPI_INIT                               0

/* allow the driver to fully erase the OctoSPI chip. This should be used carefully.
 * the call is blocking and takes a while. by default it is set to 0.
 */
#define LX_STM32_OSPI_ERASE                              0

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

#define LX_STM32_OSPI_CURRENT_TIME                       xTaskGetTickCount

/* Macro called after initializing the OSPI driver
 * e.g. create a semaphore used for transfer notification */
 /* USER CODE BEGIN LX_STM32_OSPI_POST_INIT */

#define LX_STM32_OSPI_POST_INIT()                        do { \
                                                          xspi_rx_semaphore = xSemaphoreCreateBinary(); \
                                                          xspi_tx_semaphore = xSemaphoreCreateBinary(); \
                                                          if ((xspi_rx_semaphore == NULL) || \
                                                              (xspi_tx_semaphore == NULL))  \
                                                         { \
                                                           return LX_ERROR; \
                                                         } \
                                                         } while(0)
/* USER CODE END LX_STM32_OSPI_POST_INIT */

/* Macro called before performing read operation */

/* USER CODE BEGIN LX_STM32_OSPI_PRE_READ_TRANSFER */

#define LX_STM32_OSPI_PRE_READ_TRANSFER(__status__)

/* USER CODE END LX_STM32_OSPI_PRE_READ_TRANSFER */

/* Define how to notify about Read completion operation */

/* USER CODE BEGIN LX_STM32_OSPI_READ_CPLT_NOTIFY */

#define LX_STM32_OSPI_READ_CPLT_NOTIFY(__status__)      do { \
                                                          if(xSemaphoreTake(xspi_rx_semaphore, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) == NULL) \
                                                          { \
                                                            __status__ = LX_ERROR; \
                                                          } \
                                                        } while(0)

/* USER CODE END LX_STM32_OSPI_READ_CPLT_NOTIFY */

/* Macro called after performing read operation */

/* USER CODE BEGIN LX_STM32_OSPI_POST_READ_TRANSFER */

#define LX_STM32_OSPI_POST_READ_TRANSFER(__status__)

/* USER CODE END LX_STM32_OSPI_POST_READ_TRANSFER */

/* Macro for read error handling */
/* USER CODE BEGIN LX_STM32_OSPI_READ_TRANSFER_ERROR */

#define LX_STM32_OSPI_READ_TRANSFER_ERROR(__status__)

/* USER CODE END LX_STM32_OSPI_READ_TRANSFER_ERROR */

/* Macro called before performing write operation */

/* USER CODE BEGIN LX_STM32_OSPI_PRE_WRITE_TRANSFER */

#define LX_STM32_OSPI_PRE_WRITE_TRANSFER(__status__)

/* USER CODE END LX_STM32_OSPI_PRE_WRITE_TRANSFER */

/* Define how to notify about write completion operation */

/* USER CODE BEGIN LX_STM32_OSPI_WRITE_CPLT_NOTIFY */

#define LX_STM32_OSPI_WRITE_CPLT_NOTIFY(__status__)     do { \
                                                          if(xSemaphoreTake(xspi_tx_semaphore, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) == NULL) \
                                                          { \
                                                            __status__ = LX_ERROR; \
                                                          } \
                                                        } while(0)

/* USER CODE END LX_STM32_OSPI_WRITE_CPLT_NOTIFY */

/* Macro called after performing write operation */

/* USER CODE BEGIN LX_STM32_OSPI_POST_WRITE_TRANSFER */

#define LX_STM32_OSPI_POST_WRITE_TRANSFER(__status__)

/* USER CODE END LX_STM32_OSPI_POST_WRITE_TRANSFER */

/* Macro for write error handling */

/* USER CODE BEGIN LX_STM32_OSPI_WRITE_TRANSFER_ERROR */

#define LX_STM32_OSPI_WRITE_TRANSFER_ERROR(__status__)

/* USER CODE END LX_STM32_OSPI_WRITE_TRANSFER_ERROR */

/* Exported functions prototypes ---------------------------------------------*/
INT lx_stm32_ospi_lowlevel_init(UINT instance);
INT lx_stm32_ospi_lowlevel_deinit(UINT instance);

INT lx_stm32_ospi_get_status(UINT instance);
INT lx_stm32_ospi_get_info(UINT instance, ULONG *block_size, ULONG *total_blocks);

INT lx_stm32_ospi_read(UINT instance, ULONG *address, ULONG *buffer, ULONG words);
INT lx_stm32_ospi_write(UINT instance, ULONG *address, ULONG *buffer, ULONG words);

INT lx_stm32_ospi_erase(UINT instance, ULONG block, ULONG erase_count, UINT full_chip_erase);
INT lx_stm32_ospi_is_block_erased(UINT instance, ULONG block);

UINT lx_ospi_driver_system_error(UINT error_code);

UINT lx_stm32_ospi_initialize(LX_NOR_FLASH *nor_flash);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

#define LX_STM32_OSPI_DUMMY_CYCLES_READ_OCTAL     20
#define LX_STM32_OSPI_DUMMY_CYCLES_CR_CFG         MX25LM51245G_CR2_DC_6_CYCLES

#define LX_STM32_OSPI_SECTOR_SIZE                 MX25LM51245G_SECTOR_64K
#define LX_STM32_OSPI_FLASH_SIZE                  MX25LM51245G_FLASH_SIZE
#define LX_STM32_OSPI_PAGE_SIZE                   MX25LM51245G_PAGE_SIZE

#define LX_STM32_OSPI_BULK_ERASE_MAX_TIME         MX25LM51245G_BULK_ERASE_MAX_TIME
#define LX_STM32_OSPI_SECTOR_ERASE_MAX_TIME       MX25LM51245G_SECTOR_ERASE_MAX_TIME
#define LX_STM32_OSPI_WRITE_REG_MAX_TIME          MX25LM51245G_WRITE_REG_MAX_TIME

#define LX_STM32_OSPI_OCTAL_BULK_ERASE_CMD        MX25LM51245G_OCTA_BULK_ERASE_CMD
#define LX_STM32_OSPI_OCTAL_SECTOR_ERASE_CMD      MX25LM51245G_OCTA_SECTOR_ERASE_64K_CMD

#define LX_STM32_OSPI_WRITE_ENABLE_CMD            MX25LM51245G_WRITE_ENABLE_CMD
#define LX_STM32_OSPI_WRITE_CFG_REG2_CMD          MX25LM51245G_WRITE_CFG_REG2_CMD
#define LX_STM32_OSPI_OCTAL_WRITE_ENABLE_CMD      MX25LM51245G_OCTA_WRITE_ENABLE_CMD
#define LX_STM32_OSPI_OCTAL_WRITE_CFG_REG2_CMD    MX25LM51245G_OCTA_WRITE_CFG_REG2_CMD

#define LX_STM32_OSPI_READ_STATUS_REG_CMD         MX25LM51245G_READ_STATUS_REG_CMD
#define LX_STM32_OSPI_READ_CFG_REG2_CMD           MX25LM51245G_WRITE_CFG_REG2_CMD

#define LX_STM32_OSPI_OCTAL_READ_DTR_CMD          MX25LM51245G_OCTA_READ_DTR_CMD
#define LX_STM32_OSPI_OCTAL_READ_CFG_REG2_CMD     MX25LM51245G_OCTA_READ_CFG_REG2_CMD
#define LX_STM32_OSPI_OCTAL_READ_STATUS_REG_CMD   MX25LM51245G_OCTA_READ_STATUS_REG_CMD

#define LX_STM32_OSPI_RESET_ENABLE_CMD            MX25LM51245G_RESET_ENABLE_CMD
#define LX_STM32_OSPI_RESET_MEMORY_CMD            MX25LM51245G_RESET_MEMORY_CMD

#define LX_STM32_OSPI_OCTAL_PAGE_PROG_CMD         MX25LM51245G_OCTA_PAGE_PROG_CMD

#define LX_STM32_OSPI_CR2_REG3_ADDR               MX25LM51245G_CR2_REG3_ADDR
#define LX_STM32_OSPI_CR2_REG1_ADDR               MX25LM51245G_CR2_REG1_ADDR
#define LX_STM32_OSPI_SR_WEL                      MX25LM51245G_SR_WEL
#define LX_STM32_OSPI_SR_WIP                      MX25LM51245G_SR_WIP
#define LX_STM32_OSPI_CR2_SOPI                    MX25LM51245G_CR2_SOPI
#define LX_STM32_OSPI_CR2_DOPI                    MX25LM51245G_CR2_DOPI

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* LX_STM32_OSPI_DRIVER_H */
