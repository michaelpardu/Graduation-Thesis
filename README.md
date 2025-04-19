# Graduation Thesis Project: 1-Lead ECG Signal Acquisition and Visualization

This project is a comprehensive system for acquiring, processing, and visualizing 1-lead ECG signals. It consists of three main components:

1. **Embedded System**: Handles ECG signal acquisition and data transmission.
2. **Node.js Server**: Manages data storage, retrieval, and API endpoints.
3. **React.js UI**: Provides a user-friendly interface for visualizing and analyzing ECG data.

---

## Features

### Embedded System
- Real-time ECG signal acquisition using ESP32.
- Data buffering and transmission to the server.
- NTP-based timestamp synchronization.
- Queue-based data handling for reliability.

### Node.js Server
- RESTful API for data storage and retrieval.
- API key and UID-based authentication.
- Hierarchical log storage by UID and date.
- CORS support for cross-origin requests.
- Error handling for invalid requests.

### React.js UI
- Responsive line chart for ECG data visualization.
- Kalman filter for data smoothing.
- Zoom and scroll functionality for detailed analysis.
- Light and dark mode toggle.
- User authentication with UID and API key.

---

## Installation and Setup

### 1. Embedded System
1. Install the required libraries for ESP32 (e.g., `WiFi`, `HTTPClient`, `freertos`).
2. Configure WiFi credentials and server URL in the `LVTN.ino` file.
3. Upload the code to the ESP32 using the Arduino IDE.

### 2. Node.js Server
1. Navigate to the `Nodejs server` directory:
   ```bash
   cd Nodejs server
   ```
2. Install dependencies:
   ```bash
   npm install
   ```
3. Create a `config.json` file with the following structure:
   ```json
   {
       "apiKeys": ["your-api-key"],
       "users": ["user1", "user2"]
   }
   ```
4. Start the server:
   ```bash
   node Server.js
   ```

### 3. React.js UI
1. Navigate to the `Reactjs UI` directory:
   ```bash
   cd Reactjs UI
   ```
2. Install dependencies:
   ```bash
   npm install
   ```
3. Create a `.env` file with the following environment variables:
   ```
   REACT_APP_API_KEY=your_api_key
   REACT_APP_UID=your_uid
   ```
4. Start the development server:
   ```bash
   npm start
   ```

---

## Usage

### Embedded System
- The ESP32 collects ECG data and sends it to the server at regular intervals.

### Node.js Server
- **POST Request**: Save logs.
  - URL: `http://localhost:3800`
  - Headers: `x-api-key`, `x-uid`
  - Body:
    ```json
    {
        "values": [1, 2, 3, ...]
    }
    ```
- **GET Request**: Retrieve logs.
  - URL: `http://localhost:3800?date=YYYY-MM-DD`
  - Headers: `x-api-key`, `x-uid`

### React.js UI
1. Log in using your UID and API key.
2. Visualize ECG data in the chart.
3. Use controls to zoom, scroll, and apply filters.
4. Export data as CSV for further analysis.

---

## Dependencies

- **Embedded System**: ESP32, Arduino libraries.
- **Node.js Server**: Node.js, Express.
- **React.js UI**: React, Recharts, rc-slider, js-cookie.

---

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

---

## Contributors

- **Trần Quốc Duy**
- **Nguyễn Mạnh Ba**

Supervised by **Dr. Nguyễn Văn Khanh** at **Trường Đại học Cần Thơ - Khoa Tự Động Hóa**.
