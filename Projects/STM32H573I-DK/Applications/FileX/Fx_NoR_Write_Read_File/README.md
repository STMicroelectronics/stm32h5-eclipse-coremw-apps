
## <b>Fx_NoR_Write_Read_File Application Description</b>

This application provides an example of Eclipse FileX and LevelX stacks usage on STM32H573I-DK board, it demonstrates how to create

a Fat File system on the NOR flash using FileX alongside LevelX. The application is designed to execute file operations on the MX25LM51245G

NOR flash device, the code provides all required software code for properly managing it.

The application starts by calling the FreeRTOS initialization routine which executes the main task that handles file operations. At this stage,

all FileX resources are created, the MX25LM51245G driver is initialized and a single task is created:

  - fx_app_task (Prio : 10) used for file operations.

The fx_app_task will start by formatting the NOR Flash using FileX services. The resulting file system is a FAT32 compatible, with 512 bytes per sector

and 8 sectors per cluster. The NOR flash should be erased prior to format either by the application or by the STM32CubeProgrammer, this allows LevelX and FileX to create a clean FAT FileSystem.

Chip erase operation takes considerable time when done by the application, therefore it is disabled by default.

To enable it, please define  the flag  ``LX_STM32_OSPI_ERASE`` to <b> 1 </b> in "lx_stm32_ospi_driver.h":
````
#define LX_STM32_OSPI_ERASE                              1
````

Upon successful opening of the flash media, FileX continues with creating a file called "STM32.TXT" into the root directory, then writes into it

some dummy data. Then file is re-opened in read only mode and content is checked.

Through all the steps, FileX/LevelX services are called to print the flash size available before and after the example file is written into the flash.

The number of occupied sectors is also shown.

#### <b>Expected success behavior</b>

Successful operation is marked by a toggling green LED light.

Also, information regarding the total and available size of the flash media is printed to the serial port.

#### <b>Error behaviors</b>

On failure, the red LED starts toggling while the green LED is switched off.

#### <b>Assumptions if any</b>
None

#### <b>Known limitations</b>
None

### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

3. The OCTOSPI1_IRQn NVIC priority should be lower, i.e greater in value, than the
   FreeRTOS configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY config param.

#### <b>FreeRTOS usage hints</b>
 
The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the
OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.
 
For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

#### <b>FileX/LevelX usage hints</b>

- When calling the fx_media_format() API, it is highly recommended to understand all the parameters used by the API to correctly generate a valid file system.


### <b>Keywords</b>

RTOS, FreeRTOS, FileX, LevelX, File System, NOR, OCTOSPI, XSPI, FAT32

### <b>Hardware and Software environment</b>

  - This application runs on STM32H573xx devices.
  - This application has been tested with STMicroelectronics STM32H573I-DK boards revision: MB1677-H573I-C01
    and can be easily tailored to any other supported device and development board.

  - This application uses USART1 to display logs, the hyperterminal configuration is as follows:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - Stop Bit = 1
    - Parity = none
    - Flow control = None

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open EWARM project
 - Rebuild all files and load your image into target memory
 - Run the application
