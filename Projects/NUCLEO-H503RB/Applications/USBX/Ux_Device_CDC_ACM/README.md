## <b>Ux_Device_CDC_ACM Application Description </b>

This application provides an example of Eclipse USBX stack usage on NUCLEO-H503RB board, it shows how to develop USB Device communication Class "CDC_ACM" based application.
The application is designed to emulate a USB-to-UART bridge following the Virtual COM Port (VCP) implementation, the code provides all required device descriptors framework
and associated Class descriptor report to build a compliant USB CDC_ACM device.

The application has three threads:

  - app_ux_device_thread_entry: used to initialize USB DRD HAL PCD driver and start the device.
  - usbx_cdc_acm_read_thread_entry: used to Read the received data from Virtual COM Port.
  - usbx_cdc_acm_write_thread_entry: used to send the received data over UART.

During enumeration phase, three communication pipes "endpoints" are declared in the CDC class implementation :

 - 1 x Bulk IN endpoint for receiving data from STM32 device to PC host:
   When data are received over UART they are saved in the buffer "UserTxBufferFS". Periodically, in a
   usbx_cdc_acm_write_thread_entry the state of the buffer "UserTxBufferFS" is checked. If there are available data, they
   are transmitted in response to IN token otherwise it is NAKed.

 - 1 x Bulk OUT endpoint for transmitting data from PC host to STM32 device:
   When data are received through this endpoint they are saved in the buffer "UserRxBufferFS" then they are transmitted
   over UART using DMA mode and in meanwhile the OUT endpoint is NAKed.
   Once the transmission is over, the OUT endpoint is prepared to receive next packet in HAL_UART_RxCpltCallback().

 - 1 x Interrupt IN endpoint for setting and getting serial-port parameters:
   When control setup is received, the corresponding request is executed in USBD_CDC_ACM_ParameterChange.

In this application, two requests are implemented:

    - Set line: Set the bit rate, number of Stop bits, parity, and number of data bits
    - Get line: Get the bit rate, number of Stop bits, parity, and number of data bits
   The other requests (send break, control line state) are not implemented.

<b>Notes</b>

- Receiving data over UART is handled by interrupt while transmitting is handled by DMA allowing hence the application to receive
data at the same time it is transmitting another data (full- duplex feature).

- The user has to check the list of the COM ports in Device Manager to find out the COM port number that have been assigned (by OS) to the VCP interface.
- CDC ACM non-blocking transmission by default disabled, to enable non-blocking transmission UX_DEVICE_CLASS_CDC_ACM_TRANSMISSION_DISABLE must be disabled.

#### <b>Expected success behavior</b>

When plugged to PC host, the NUCLEO-H503RB must be properly enumerated as an USB Serial device and an STlink Com port.
During the enumeration phase, the device must provide host with the requested descriptors (Device descriptor, configuration descriptor, string descriptors).
Those descriptors are used by host driver to identify the device capabilities. Once NUCLEO-H503RB USB device successfully completed the enumeration phase,
Open two hyperterminals (USB com port and UART com port(USB STLink VCP)) to send/receive data to/from host from/to device.

#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (CDC Device enumeration failed, PC and Device can not communicate over VCP ports).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" Specification and CDC_ACM class Specification.

#### <b> Known limitations</b>

None

#### <b>FreeRTOS usage hints</b>

The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the
OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

### <b>Keywords</b>

RTOS, USBX, USBXDevice, USB_DRD, Full Speed, CDC, VCP, USART, DMA.

### <b>Hardware and Software environment</b>

  - This application runs on STM32H5xx devices
  - This application has been tested with STMicroelectronics NUCLEO-H503RB boards Revision MB1814-B02.
    and can be easily tailored to any other supported device and development board.

  - NUCLEO-H503RB Set-up:
    - Connect the NUCLEO-H503RB board CN3 to the PC through "TYPE-C" to "Standard A" cable.
    - For VCP the configuration is dynamic for example it can be:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = None
      - Flow control = None

  - The USART3 interface available on PA3 and PA4 of the microcontroller are connected to ST-LINK MCU.
  - By default the USART3 communication between the target MCU and ST-LINK MCU is enabled. It's configuration is as following:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - Stop Bit = 1
    - Parity = None
    - Flow control = None

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open EWARM project
 - Rebuild all files and load your image into target memory
 - Run the application
