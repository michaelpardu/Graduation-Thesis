const http = require("http");
const fs = require("fs");
const path = require("path");

// Custom implementation of the IDCT function
function idct(input) {
    const N = 500; // Ensure 500 samples
    const paddedInput = new Array(N).fill(0); // Initialize with zeros
    for (let i = 0; i < input.length && i < N; i++) {
        paddedInput[i] = input[i]; // Copy input data
    }

    const output = new Array(N).fill(0);
    const factor = Math.PI / N;

    for (let k = 0; k < N; k++) {
        let sum = 0;
        for (let n = 0; n < N; n++) {
            const coefficient = n === 0 ? Math.sqrt(1 / N) : Math.sqrt(2 / N);
            sum += coefficient * paddedInput[n] * Math.cos(factor * n * (k + 0.5));
        }
        output[k] = sum;
    }

    return output;
}

const logFile = fs.createWriteStream(path.join(__dirname, "server.log"), { flags: "a" }); // Append mode

// Override console.log to log to both terminal and file
const originalLog = console.log;
console.log = (...args) => {
    const message = `[${getCurrentTimestamp()}] ${args.join(" ")}`;
    originalLog(message); // Log to terminal
    logFile.write(message + "\n"); // Log to file
};

// Override console.error to log to both terminal and file
const originalError = console.error;
console.error = (...args) => {
    const message = `[${getCurrentTimestamp()}] ERROR: ${args.join(" ")}`;
    originalError(message); // Log to terminal
    logFile.write(message + "\n"); // Log to file
};

// Function to get the log file name
function getLogFileName() {
    return "logs.json"; // Single JSON file for all logs
}

// Function to get the log file path for a specific UID and date
function getLogFilePath(uid, date) {
    const [year, month, day] = date.split("-");
    const userFolder = path.join(__dirname, "logs", uid); // Folder for the specific UID
    if (!fs.existsSync(userFolder)) {
        fs.mkdirSync(userFolder, { recursive: true }); // Create folder if it doesn't exist
    }
    return path.join(userFolder, `${month}-${day}-${year}.json`);
}

// Function to load API keys and users from a JSON file
function loadApiKeysAndUsers() {
    const configPath = path.join(__dirname, "config.json");
    try {
        const configData = fs.readFileSync(configPath, "utf8");
        return JSON.parse(configData);
    } catch (err) {
        console.error(`[${getCurrentTimestamp()}] Error loading config.json:`, err);
        return { apiKeys: [], users: [] }; // Default empty values
    }
}

// Function to validate API key
function isValidApiKey(apiKey) {
    const { apiKeys } = loadApiKeysAndUsers();
    return apiKeys.includes(apiKey);
}

// Function to validate UID
function isValidUid(uid) {
    const { users } = loadApiKeysAndUsers();
    return users.includes(uid);
}

// Function to get the current timestamp
function getCurrentTimestamp() {
    return new Date().toISOString();
}

// Create server
/**
 * Create an HTTP server to handle POST requests by writing the request content
 * into a JSON file with a hierarchical structure based on the UID and date,
 * and also log the data into a text file with timestamps.
 *
 * Additionally, handle GET requests to retrieve the logs.json file or specific parts of it.
 *
 * @param {http.IncomingMessage} req - Incoming HTTP request object.
 * @param {http.ServerResponse} res - HTTP response object.
 */
const server = http.createServer((req, res) => {
    // Add CORS headers
    res.setHeader("Access-Control-Allow-Origin", "*"); // Allow all origins
    res.setHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS"); // Allowed methods
    res.setHeader("Access-Control-Allow-Headers", "Content-Type, x-api-key, x-uid"); // Allowed headers

    if (req.method === "OPTIONS") {
        // Handle OPTIONS request (preflight request)
        res.writeHead(204); // No Content
        res.end();
        console.log(`[${getCurrentTimestamp()}] OPTIONS request handled successfully`);
        return;
    }

    const apiKey = req.headers["x-api-key"];
    const uid = req.headers["x-uid"];

    if (!apiKey || !uid || !isValidApiKey(apiKey) || !isValidUid(uid)) {
        res.writeHead(403, { "Content-Type": "text/plain" });
        res.end("Forbidden: Invalid API key or UID");
        console.log(`[${getCurrentTimestamp()}] Forbidden: Invalid API key or UID`);
        return;
    }

    if (req.method === "GET") {
        // Handle GET requests to retrieve logs.json or specific parts of it
        const url = new URL(req.url, `http://${req.headers.host}`);
        const date = url.searchParams.get("date") || new Date().toISOString().split("T")[0]; // Default to today's date
        const logPath = getLogFilePath(uid, date);

        fs.readFile(logPath, "utf8", (err, data) => {
            if (err) {
                console.error(`[${getCurrentTimestamp()}] Error reading JSON file:`, err);
                res.writeHead(404, { "Content-Type": "application/json" });
                res.end(JSON.stringify({ status: "error", message: "Logs not found for the specified date" }));
                return;
            }

            try {
                const logs = JSON.parse(data);
                const dateLogs = logs[date] || []; // Extract logs for the requested date
                res.writeHead(200, { "Content-Type": "application/json" });
                res.end(JSON.stringify({ status: "success", data: dateLogs })); // Return only the data array
                console.log(`[${getCurrentTimestamp()}] Returned logs for UID ${uid} on date ${date}`);
            } catch (parseErr) {
                console.error(`[${getCurrentTimestamp()}] Error parsing JSON:`, parseErr);
                res.writeHead(500, { "Content-Type": "application/json" });
                res.end(JSON.stringify({ status: "error", message: "Server error" }));
            }
        });
    } else if (req.method === "POST") {
        // Handle POST requests to save logs
        let body = "";

        req.on("data", (chunk) => {
            body += chunk;
        });

        req.on("end", () => {
            try {
                const parsedData = JSON.parse(body); // Parse the received JSON data
                const date = new Date().toISOString().split("T")[0]; // Get today's date
                const logPath = getLogFilePath(uid, date);
                const originalLogPath = logPath.replace(".json", "_original.json"); // Path for original data

                fs.readFile(logPath, "utf8", (err, fileData) => {
                    let logs = {};
                    if (!err) {
                        try {
                            logs = JSON.parse(fileData); // Parse existing logs
                        } catch (parseErr) {
                            console.error(`[${getCurrentTimestamp()}] Error parsing existing logs:`, parseErr);
                        }
                    }

                    if (!logs[date]) {
                        logs[date] = []; // Initialize array for the date if not present
                    }

                    // Perform IDCT on the `values` array if it exists
                    if (parsedData.values && Array.isArray(parsedData.values)) {
                        const idctValues = idct(parsedData.values); // Perform IDCT
                        logs[date].push({ ...parsedData, values: idctValues }); // Save IDCT-transformed data

                        // Save raw data from the client to a separate file
                        const originalLogs = logs[date].map((entry) => ({
                            ...parsedData, // Save only raw client data
                        }));
                        fs.writeFile(originalLogPath, JSON.stringify({ [date]: originalLogs }, null, 2), (writeErr) => {
                            if (writeErr) {
                                console.error(`[${getCurrentTimestamp()}] Error writing original log file:`, writeErr);
                            }
                        });
                    } else {
                        logs[date].push(parsedData); // Save raw data if no `values` array
                    }

                    fs.writeFile(logPath, JSON.stringify(logs, null, 2), (writeErr) => {
                        if (writeErr) {
                            console.error(`[${getCurrentTimestamp()}] Error writing to log file:`, writeErr);
                            res.writeHead(500, { "Content-Type": "application/json" });
                            res.end(JSON.stringify({ status: "error", message: "Server error" }));
                            return;
                        }

                        res.writeHead(200, { "Content-Type": "application/json" });
                        res.end(JSON.stringify({ status: "success", message: "Log saved successfully" }));
                        console.log(`[${getCurrentTimestamp()}] Log saved successfully for UID ${uid} on date ${date}`);
                    });
                });
            } catch (err) {
                console.error(`[${getCurrentTimestamp()}] Error processing POST request:`, err);
                res.writeHead(400, { "Content-Type": "application/json" });
                res.end(JSON.stringify({ status: "error", message: "Invalid JSON format" }));
            }
        });
    } else {
        res.writeHead(405, { "Content-Type": "application/json" });
        res.end(JSON.stringify({ status: "error", message: "Method not allowed" }));
        console.log(`[${getCurrentTimestamp()}] Method not allowed: ${req.method}`);
    }
});

// Listen on port 3000
const PORT = 3800;
server.listen(PORT, "0.0.0.0", () => {
    console.log(`[${getCurrentTimestamp()}] Server is running at http://localhost:${PORT}`);
});
