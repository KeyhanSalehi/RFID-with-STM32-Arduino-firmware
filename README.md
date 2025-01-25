# RFID with STM32 & Arduino Firmware

This project provides a simple and efficient way to interface RFID cards with STM32 microcontrollers using the popular MFRC522 RFID library. The goal of this project is to allow seamless communication with RFID tags and to offer an easy setup for implementing RFID-based solutions.

The firmware is built using **PlatformIO**, a powerful and flexible development environment, which also supports integration with the Arduino platform.

## Features

- **Easy RFID Integration:** Interface RFID cards using the MFRC522 library.
- **STM32 Supported:** The project is specifically designed for STM32 microcontrollers, but can also be adapted to other microcontrollers that support the Arduino platform.
- **Open Source:** Free to use, modify, and distribute under the MIT license.

## Supported Microcontrollers

While the project is developed with STM32 microcontrollers in mind, it can be easily adapted to other microcontrollers that support the Arduino platform, including but not limited to:

- Arduino Uno
- Arduino Mega 2560
- Arduino Nano
- Arduino Leonardo
- Arduino Due
- ESP32
- ESP8266
- STM32 (Using STM32duino)
  
## Requirements

- **PlatformIO**: Make sure to install [PlatformIO](https://platformio.org/) in your code editor (such as VSCode).
- **MFRC522 Library**: Install the MFRC522 library via PlatformIO's library manager.
- **STM32 Support in PlatformIO**: PlatformIO supports STM32 microcontrollers via the STM32duino framework.

### For PlatformIO:

1. Install PlatformIO in your preferred code editor (e.g., [VSCode](https://code.visualstudio.com/)).
2. Open the project folder in PlatformIO.
3. Open `platformio.ini` to configure your board and upload settings.
4. Select the appropriate STM32 (or another supported microcontroller) in the `platformio.ini` configuration file.
5. Compile and upload the code using the PlatformIO toolbar in the editor.

### For Arduino IDE:

1. Download or clone the repository to your local machine.
2. Open the project folder in the Arduino IDE.
3. Install the **MFRC522 Library** via the Library Manager.
4. Select your target microcontroller (e.g., STM32 or Arduino board) in the **Tools** menu.
5. Upload the code to your microcontroller.

## Usage

Once the firmware is uploaded, connect your RFID reader (MFRC522) and RFID cards to the STM32 (or any supported microcontroller). The system will read the RFID tags and can be configured to take actions based on the tag information.
