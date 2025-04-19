# Server Application

This project is a server application designed to handle HTTP requests for logging and retrieving data. It supports both GET and POST methods and includes features like data transformation using IDCT and hierarchical log storage.

## Features

- **POST Requests**: Save logs with optional data transformation (IDCT).
- **GET Requests**: Retrieve logs based on UID and date.
- **CORS Support**: Allows cross-origin requests.
- **API Key and UID Validation**: Ensures secure access.
- **Hierarchical Log Storage**: Logs are stored in a structured format based on UID and date.
- **Error Handling**: Comprehensive error handling for invalid requests and server issues.

## Setup

1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Install dependencies (if any) using:
   ```bash
   npm install
   ```
4. Create a `config.json` file in the root directory with the following structure:
   ```json
   {
       "apiKeys": ["your-api-key"],
       "users": ["user1", "user2"]
   }
   ```
5. Start the server:
   ```bash
   node Server.js
   ```

## Usage

### POST Request
Send a POST request to save logs:
- URL: `http://localhost:3800`
- Headers:
  - `x-api-key`: Your API key.
  - `x-uid`: User ID.
- Body (JSON):
  ```json
  {
      "values": [1, 2, 3, ...]
  }
  ```

### GET Request
Retrieve logs by sending a GET request:
- URL: `http://localhost:3800?date=YYYY-MM-DD`
- Headers:
  - `x-api-key`: Your API key.
  - `x-uid`: User ID.

### Logs
Logs are stored in the `logs` directory, organized by UID and date.

## Development

- **IDCT Function**: Custom implementation for data transformation.
- **Logging**: Logs are written to both the terminal and a `server.log` file.

## License

This project is licensed under the MIT License.
