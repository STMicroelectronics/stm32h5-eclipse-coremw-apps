## <b>Ux_Device_HID Application Description</b>

This application provides an example of Eclipse USBX stack usage on STM32H563xx board,
it shows how to develop USB Device Human Interface "HID" mouse based application.

The application is designed to emulate a USB HID mouse device, the code provides all required device descriptors framework
and associated class descriptor report to build a compliant USB HID mouse device.

The application has two threads:

  - app_ux_device_thread_entry: used to initialize USB DRD HAL PCD driver and start the device.
  - usbx_hid_thread_entry: used to send HID reports to move automatically the PC host machine cursor.

To customize the HID application by sending the mouse position step by step every 10ms.
For each 10ms, the application calls the GetPointerData() API to update the mouse position (x, y) and send
the report buffer through the ux_device_class_hid_event_set() API.

#### <b>Expected success behavior</b>

When plugged to PC host, the STM32H563xx must be properly enumerated as a USB HID mouse device.
During the enumeration phase, device provides host with the requested descriptors (Device, configuration, string).
Those descriptors are used by host driver to identify the device capabilities.
Once the STM32H563xx USB device successfully completed the enumeration phase, the device sends an HID report after a user button press.
Each report sent should move the mouse cursor by one step on host side.

#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (Mouse enumeration failed, the mouse pointer doesn't move).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" Specification and HID class Specification.

#### <b>Known limitations</b>

The remote wakeup feature is not yet implemented (used to bring the USB suspended bus back to the active condition).

### <b>Notes</b>

None

#### <b>FreeRTOS usage hints</b>

The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the
OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

### <b>Keywords</b>

RTOS, FreeRTOS, USBX Device, USB_DRD, Full Speed, HID, Mouse, USBPD

### <b>Hardware and Software environment</b>

  - This application runs on STM32h563xx devices.
  - This application has been tested with STMicroelectronics STM32H563ZI boards Revision MB1404-H563ZI-C01 and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open EWARM project
 - Rebuild all files and load your image into target memory
 - Run the application
