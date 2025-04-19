# Dự án Luận Văn Tốt Nghiệp: Thu Thập và Hiển Thị Tín Hiệu ECG 1-Dẫn
# Graduation Thesis Project: 1-Lead ECG Signal Acquisition and Visualization

Dự án này là một hệ thống toàn diện để thu thập, xử lý và hiển thị tín hiệu ECG 1-dẫn. Nó bao gồm ba thành phần chính:

1. **Hệ Thống Nhúng**: Xử lý thu thập tín hiệu ECG và truyền dữ liệu.
2. **Máy Chủ Node.js**: Quản lý lưu trữ dữ liệu, truy xuất và các điểm cuối API.
3. **Giao Diện React.js**: Cung cấp giao diện thân thiện để hiển thị và phân tích dữ liệu ECG.

---

## Các Tính Năng

### Hệ Thống Nhúng
- Thu thập tín hiệu ECG theo thời gian thực bằng ESP32.
- Bộ đệm dữ liệu và truyền đến máy chủ.
- Đồng bộ hóa thời gian dựa trên NTP.
- Xử lý dữ liệu dựa trên hàng đợi để đảm bảo độ tin cậy.

### Máy Chủ Node.js
- API RESTful để lưu trữ và truy xuất dữ liệu.
- Xác thực dựa trên API key và UID.
- Lưu trữ nhật ký theo cấu trúc phân cấp dựa trên UID và ngày tháng.
- Hỗ trợ CORS cho các yêu cầu từ nguồn gốc khác.
- Xử lý lỗi cho các yêu cầu không hợp lệ.

### Giao Diện React.js
- Biểu đồ dạng đường nhạy bén để hiển thị dữ liệu ECG.
- Bộ lọc Kalman để làm mượt dữ liệu.
- Chức năng phóng to và cuộn để phân tích chi tiết.
- Chuyển đổi chế độ sáng và tối.
- Xác thực người dùng bằng UID và API key.

---

## Cài Đặt và Thiết Lập

### 1. Hệ Thống Nhúng
1. Cài đặt các thư viện cần thiết cho ESP32 (ví dụ: `WiFi`, `HTTPClient`, `freertos`).
2. Cấu hình thông tin WiFi và URL máy chủ trong tệp `LVTN.ino`.
3. Tải mã lên ESP32 bằng Arduino IDE.

### 2. Máy Chủ Node.js
1. Điều hướng đến thư mục `Nodejs server`:
   ```bash
   cd Nodejs server
   ```
2. Cài đặt các gói phụ thuộc:
   ```bash
   npm install
   ```
3. Tạo một tệp `config.json` với cấu trúc sau:
   ```json
   {
       "apiKeys": ["your-api-key"],
       "users": ["user1", "user2"]
   }
   ```
4. Khởi động máy chủ:
   ```bash
   node Server.js
   ```

### 3. Giao Diện React.js
1. Điều hướng đến thư mục `Reactjs UI`:
   ```bash
   cd Reactjs UI
   ```
2. Cài đặt các gói phụ thuộc:
   ```bash
   npm install
   ```
3. Tạo tệp `.env` với các biến môi trường sau:
   ```
   REACT_APP_API_KEY=your_api_key
   REACT_APP_UID=your_uid
   ```
4. Khởi động máy chủ phát triển:
   ```bash
   npm start
   ```

---

## Hướng Dẫn Sử Dụng

### Hệ Thống Nhúng
- ESP32 thu thập dữ liệu ECG và gửi đến máy chủ theo khoảng thời gian định kỳ.

### Máy Chủ Node.js
- **Yêu Cầu POST**: Lưu nhật ký.
  - URL: `http://localhost:3800`
  - Headers: `x-api-key`, `x-uid`
  - Body:
    ```json
    {
        "values": [1, 2, 3, ...]
    }
    ```
- **Yêu Cầu GET**: Truy xuất nhật ký.
  - URL: `http://localhost:3800?date=YYYY-MM-DD`
  - Headers: `x-api-key`, `x-uid`

### Giao Diện React.js
1. Đăng nhập bằng UID và API key của bạn.
2. Hiển thị dữ liệu ECG trên biểu đồ.
3. Sử dụng các điều khiển để phóng to, cuộn và áp dụng bộ lọc.
4. Xuất dữ liệu dưới dạng CSV để phân tích thêm.

---

## Phụ Thuộc

- **Hệ Thống Nhúng**: ESP32, thư viện Arduino.
- **Máy Chủ Node.js**: Node.js, Express.
- **Giao Diện React.js**: React, Recharts, rc-slider, js-cookie.

---

## Giấy Phép

Dự án này được cấp phép theo Giấy Phép MIT. Xem tệp `LICENSE` để biết thêm chi tiết.

---

## Người Đóng Góp

- **Trần Quốc Duy**
- **Nguyễn Mạnh Ba**

Được hướng dẫn bởi **TS. Nguyễn Văn Khánh** tại **Trường Đại học Cần Thơ - Khoa Tự Động Hóa**.

# English

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
