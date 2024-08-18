# STM32F401 MIDI USB Device Controller

[![en](https://img.shields.io/badge/lang-en-red.svg)](README.md) [![pt-br](https://img.shields.io/badge/lang-pt--br-green.svg)](README.pt-br.md)


A MIDI controller demo based on STM32F401CDT6

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

***

## Description

This project is just a demonstration of using STM32F4 USB interface as a MIDI device. It receives and transmitt MIDI messages of the standard types:
- Note on and off
- Program Change
- Control Change

The implementation of the MIDI class for the STM32F401's USB Device interface is the same as that available in the STM32 MIDI Brain project (https://github.com/samjkent/stm32f4-midi-brain) with a small bug fix. Of this code, what I changed most was the user interface (files `usbd_midi_if.c` and `usbd_midi_if.h`). I included data structures and callback functions for receiving MIDI messages. I've also updated the functions for sending MIDI messages.

In the main program (file `main.c`) you can check how MIDI messages are read and written. The functions for this are:
- Sending MIDI messages:
  - `MIDI_SendNote`: sends a note
  - `MIDI_SendProgramChange` sends a Program Change message and,
  - `MIDI_SendControlChange` sends a Controller Change message.
- For receiving, there are 3 Callbacks that must be defined by the user (they are initialy defined as `weak`):
  - `MIDI_ReceivedNoteCallback` for receiving a MIDI note. 
  - `MIDI_ReceivedProgramChangeCallback` for Program Change messages.
  - `MIDI_ReceivedControlChangeCallback` for Controller Change messages.

The `usbd_midi_if.h` file defines the types of variables used by these functions (they are unions).

The source code project is built based on the ST libraries, using STM32CubeIDE version 1.16.0. For this demo I used the board

## How to Test the Code

I'll explain below how you can compile the code so that you have a USB MIDI device for a microcontroller from the STM32F4 family.

### Option 1: importing the project

The quickest way for you to test this code and use the USB MIDI Class is to import the project into your STM32CubeIDE workspace. You can do this with the Import Existing Projects into Workspace function in STM32CubeIDE.

I used version 1.16.0 of STM32CubeIDE.

### Option 2: using the MIDI Class in your project

If you already have a project in STM32CubeIDE and just want to use the MIDI class, I've included below a step-by-step guide on how to generate the project in STM32CubeIDE so that you can compile and test the code without having to import my project. I believe it works for any STM32F4 that has a USB Device interface using the CubeMX libraries.

1. Use your project or, if you prefer, create a new project in STM32CubeIDE and run some simple firmware, such as a Blink LED, to make sure everything is working (clock configuration, debug interface and everything else).
2. Using the Device Configuration Tool, in the Connectivity category, select `USB_OTG_FS` and set it to `Device_Only` mode. In NVIC Settings, enable the USB interface interrupt.
3. In the Middleware and Software Packs category, select `USB_DEVICE` and choose a Class For FS IP. I chose `Custom Human Interface Device Class (HID)`. I believe that other classes can be chosen, as they will not be used. The MIDI class will be added manually in the sequence, so we'll baypass the class chosen here. In the `Parameter Settings` fields you can configure the number of interfaces (I've left the default value) and in Device Descriptor you can change the manufacturer, product and configuration strings.
4. Check the Clock Configuration in the Device Configuration Tool. The USB device requires a 48MHz clock. If you use an **STM32F401 with a 25MHz cristal oscillator** you can use the following clock configuration values:
 5. PLL Source MUX: HSE
 1. PLL / M: 25
 2. PLL * N: 336
 3. PLL / P: 4
 4. PLL / Q: 7
 5. System Clock MUX: PLLCLK
 5. AHB Prescaler = 1
5. Generate the code and save.
4. Copy the MIDI folder in `Middlewares/ST/STM32_USB_Device_Library/Class` to the respective folder in your project.
5. Add the `Middlewares/ST/STM32_USB_Device_Library/Class/MIDI/Inc` folder to the list of includes in your project's properties.
6. Copy the `usbd_midi_if.c` and `usbd_midi_if.h` files that are in the `/USB_DEVICE/App` folder to the respective folder in your project.
7. Edit the `usb_device.c` file in the `/USB_DEVICE/App` folder. 
At the beginning, include the header for the user program interface:
```c
/* USER CODE BEGIN Includes */
#include "usbd_midi_if.h" // Add this line.
/* USER CODE END Includes */
```
Change the `void MX_USB_DEVICE_Init(void)` function to include:
```c
  /* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */
  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
  {
	Error_Handler();
  }
  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_MIDI) != USBD_OK)
  {
	Error_Handler();
  }
  if (USBD_MIDI_RegisterInterface(&hUsbDeviceFS, &USBD_MIDI_fops) != USBD_OK)
  {
    Error_Handler();
  }
  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
  {
	Error_Handler();
  }
  return;
  /* USER CODE END USB_DEVICE_Init_PreTreatment */
```
With this code, the function returns before registering the chosen class with the Device Configuration Tool, so this is the baypass mantioned above.

You can now compile and test. When you connect your device to the computer's USB interface, it will be recognized as a MIDI device. In my case, the `lsusb` command in linux recognizes the microcontroller as `0483:5750 STMicroelectronics LED badge -- mini LED display -- 11x44`. This is due to the PID and VID configured in the Device Descriptor, which are registered by the ST.

In linux you can use the program **midisnoop** to check the MIDI messages sent from the USB Device.