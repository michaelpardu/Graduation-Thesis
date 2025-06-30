# 🎓 Graduation Thesis Project: 1-Lead ECG Signal Acquisition and Visualization

![ECG Signal Visualization](https://img.shields.io/badge/ECG%20Signal%20Visualization-Project-brightgreen)

Welcome to the **Graduation Thesis Project**! This repository focuses on the acquisition and visualization of 1-lead ECG signals. The project integrates hardware and software components to provide a comprehensive solution for ECG signal processing.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Technologies Used](#technologies-used)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Releases](#releases)

## Introduction

Electrocardiography (ECG) is a vital tool in monitoring heart health. This project aims to create a user-friendly system for acquiring and visualizing ECG signals using a single lead. The combination of ESP32 hardware and various software technologies allows for high-speed sampling and real-time data visualization.

## Features

- **Real-time ECG Signal Acquisition**: Capture ECG signals with high accuracy.
- **Visualization**: Display ECG waveforms in real-time for immediate analysis.
- **User-friendly Interface**: Built with ReactJS for easy interaction.
- **Data Processing**: Utilize Python for backend processing of the ECG data.
- **High-speed Sampling**: Leverage the ESP32's capabilities for efficient data handling.

## Technologies Used

This project employs a variety of technologies to achieve its goals:

- **Hardware**: ESP32 microcontroller for data acquisition.
- **Programming Languages**: 
  - Python 3 for backend processing.
  - JavaScript for frontend development.
- **Frameworks**: 
  - ReactJS for building the user interface.
- **Libraries**: 
  - Cosine LUT for signal processing.
  - I2S ADC for high-speed sampling.

## Installation

To set up this project, follow these steps:

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/michaelpardu/Graduation-Thesis.git
   ```

2. **Install Dependencies**:
   Navigate to the project directory and install the required packages:
   ```bash
   cd Graduation-Thesis
   npm install
   ```

3. **Setup the ESP32**:
   - Follow the instructions provided in the hardware setup guide to configure the ESP32 for ECG signal acquisition.

4. **Run the Application**:
   Start the application by executing:
   ```bash
   npm start
   ```

## Usage

Once the application is running, you can access it through your web browser. The user interface will allow you to start the ECG signal acquisition and visualize the data in real-time.

1. **Connect the ESP32**: Ensure that the ESP32 is connected to your computer.
2. **Start Acquisition**: Click on the "Start" button to begin capturing ECG signals.
3. **View Data**: The ECG waveform will display in real-time on the screen.

For additional details, refer to the [Releases](https://github.com/michaelpardu/Graduation-Thesis/releases) section for downloadable files and updates.

## Contributing

Contributions are welcome! If you have suggestions for improvements or want to add features, feel free to fork the repository and submit a pull request. Please ensure that your code adheres to the existing style and includes appropriate tests.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

## Contact

For questions or feedback, you can reach me at:

- Email: [your-email@example.com](mailto:your-email@example.com)
- GitHub: [michaelpardu](https://github.com/michaelpardu)

## Releases

For the latest updates and downloadable files, visit the [Releases](https://github.com/michaelpardu/Graduation-Thesis/releases) section. Here, you can find the compiled versions of the project, along with release notes detailing new features and fixes.

![Release Button](https://img.shields.io/badge/Download%20Releases-Click%20Here-blue)

## Conclusion

This project serves as a valuable resource for anyone interested in ECG signal acquisition and visualization. By combining hardware and software, it provides a robust solution for monitoring heart health. Thank you for exploring the **Graduation Thesis Project**!