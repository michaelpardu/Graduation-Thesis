# Graduation Thesis - Embedded System Project

This project is an embedded system application designed to collect, process, and transmit analog data using an ESP32 microcontroller. The system utilizes I2S for high-speed ADC sampling, performs Discrete Cosine Transform (DCT) on the data, and sends the processed data to a remote server.

## Features
- High-speed ADC sampling using I2S.
- Real-time data processing with DCT.
- Data storage on an SD card.
- Data transmission to a remote server via WiFi.
- Time synchronization using NTP.

## Components
- **ESP32**: Microcontroller for data acquisition and processing.
- **SD Card**: For local data storage.
- **WiFi**: For remote data transmission.
- **DCTLibrary**: Custom library for performing DCT.

## File Structure
- `LVTN.ino`: Main application code.
- `DCTLibrary.h` and `DCTLibrary.cpp`: DCT computation logic.
- `dct_lut.h`: Lookup table for DCT coefficients.

## Setup Instructions
1. **Hardware Setup**:
   - Connect an analog sensor to GPIO34 (ADC1_CHANNEL_6).
   - Insert an SD card into the SD_MMC slot.
   - Connect an LED to GPIO33 for status indication.

2. **Software Setup**:
   - Install the Arduino IDE and ESP32 board support.
   - Clone this repository to your local machine.
   - Open `LVTN.ino` in the Arduino IDE.
   - Update the WiFi credentials (`ssid` and `password`) in the code.

3. **Dependencies**:
   - Install the required libraries:
     - `WiFi.h`
     - `HTTPClient.h`
     - `FS.h`
     - `SD_MMC.h`
     - `freertos/FreeRTOS.h`

4. **Upload Code**:
   - Select the correct ESP32 board and port in the Arduino IDE.
   - Compile and upload the code to the ESP32.

## Usage
- The system will start collecting data after booting.
- Processed data is stored on the SD card and sent to the server.
- The LED indicates the WiFi connection status:
  - **ON**: Disconnected.
  - **OFF**: Connected.

## Notes
- Ensure the SD card is formatted as FAT32.
- The server URL and API keys are hardcoded in the code. Update them as needed.

## License
This project is for educational purposes and is licensed under the MIT License.