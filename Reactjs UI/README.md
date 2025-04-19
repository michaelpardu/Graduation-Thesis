# 1-Lead ECG by EDS

![1-Lead ECG by EDS](public/eds-dark-logo.svg)

A React-based application for visualizing 1-lead ECG data in a chart format. The app supports data filtering using a Kalman filter and allows zooming and scrolling through the data.

## Features

- Fetch data from a remote API.
- Visualize data using a responsive line chart.
- Apply a Kalman filter to smooth the data.
- Zoom and scroll through the chart.
- Adjustable parameters for the Kalman filter.
- Dark mode toggle and logout functionality.

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/your-repo/1-lead-ecg.git
   cd 1-lead-ecg
   ```

2. Install dependencies:
   ```bash
   npm install
   ```

3. Create a `.env` file in the root directory and add the following environment variables:
   ```
   REACT_APP_API_KEY=your_api_key
   REACT_APP_UID=your_uid
   ```

## Usage

1. Start the development server:
   ```bash
   npm start
   ```

2. Open the app in your browser at `http://localhost:3000`.

3. Log in using your `UID` and `API Key`.

4. Use the controls to:
   - Navigate through the data (`Prev` and `Next` buttons).
   - Adjust the zoom level.
   - Change the number of visible data points.
   - Modify Kalman filter parameters (`Q`, `R`, `P`, `K`) and apply the filter.

5. Toggle between light and dark mode using the toggle button.

6. Log out using the logout button next to the `UID`.

## Dependencies

- React
- Recharts
- js-cookie
- rc-slider

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
