
## <b>Fx_uSD_File_Edit Application Description</b>

This application provides an example of Eclipse FileX stack usage on STM32H573I-DK board, it shows how to develop a basic SD card file operations application.

The application is designed to handle SD card insertion/removal events, and depending on that state, it starts and stops file operations from and into the SD card.

The application is developed using HAL, so managing LEDs, SD initialisation or any SD card insertion/removal mechanisms are implemented using HAL drivers without any reference to BSP.

A single thread is created:

  - fx_app_thread: used to initialize the SD driver and starting FileX's file operations.

A message queue is used to signal the SD card detection event to the fx_app_thread thread:

  - msg_queue: used to notify the fx_app_thread about the SD card insertion status.

The fx_app_thread starts by checking whether the SD card is initially inserted or not. If it is, it sends a message to the queue to ensure

that the first iteration starts properly. The wait on the message queue blocks until it receives a new message indicating whether the SD card has been inserted

or removed. Interrupt callback for SD detection is registered and it is used to send the event information to the message queue.

The fx_app_thread uses FileX services to open the SD media for file operations and attempt to create file STM32.TXT. If the file already exists,

it will be overwritten. Dummy content is then written into the file and it is closed. The file is opened once again in read mode and content

is checked if matches what was previously written.

It is possible to unplug/plug or replace the SD card without any need to reset the STM32H573I-DK.
That is why MX_SDMMC1_SD_Init() should be public to initialize the newly plugged SD card.

#### <b>Expected success behavior</b>

- A file named STM32.TXT should be visible in the root directory of the SD card.
- A blinking green LED light marks the success of the file operations.

#### <b>Error behaviors</b>

- On failure, the red LED should start blinking while the green LED is turned off.
- Error handler is called at the spot where the error occurred.

#### <b>Assumptions if any</b>
- The SD card should be plugged prior to run the application.

#### <b>Known limitations</b>

Performing quick plug/unplug of SD card may trigger the Error_Handler() function.

### <b>Notes</b>

 1. Before starting the application, the SD card should be present in the SD card connector and formatted initially by user.

#### <b>FreeRTOS usage hints</b>

The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the
OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

The SDMMC1_IRQn interrupt must have lower priority (numerically higher) than the configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY.

#### <b>FileX/LevelX usage hints</b>

- When calling the fx_media_format() API, it is highly recommended to understand all the parameters used by the API to correctly generate a valid filesystem.


### <b>Keywords</b>

RTOS, FreeRTOS, FileX, File system, SDMMC, FAT32

### <b>Hardware and Software environment</b>

  - This application runs on STM32H573xx devices.
  - This application has been tested with STMicroelectronics STM32H573I-DK boards revision: MB1677-H573I-C01
    and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open EWARM project
 - Rebuild all files and load your image into target memory
 - Run the application
