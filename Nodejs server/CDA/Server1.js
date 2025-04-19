const http = require("http");
const fs = require("fs");
const path = require("path");

const PORT = 3000;
const LOG_FILE = path.join(__dirname, "logs.json");

// Hàm ghi log vào file JSON (update nếu có, chỉ tạo mới khi khác ngày)
function saveLog(data) {
    let logs = {};

    try {
        if (fs.existsSync(LOG_FILE)) {
            logs = JSON.parse(fs.readFileSync(LOG_FILE, "utf8"));
        }
    } catch (err) {
        console.error("Lỗi khi đọc file log:", err);
    }

    // Lấy thông tin từ dữ liệu mới
    const uid = data.UID || "unknown";
    const station = Object.keys(data.Station)[0]; // Lấy số Station đầu tiên
    const date = Object.keys(data.Station[station])[0]; // Lấy ngày đầu tiên
    const number = data.Station[station][date].Number;

    // Nếu UID chưa tồn tại, tạo mới
    if (!logs[uid]) logs[uid] = {};
    if (!logs[uid][station]) logs[uid][station] = {};
    
    // Nếu ngày đã tồn tại, update số mới
    if (logs[uid][station][date]) {
        logs[uid][station][date].Number = number;
    } else {
        // Nếu chưa có ngày này, thêm mới
        logs[uid][station][date] = { Number: number };
    }

    // Ghi dữ liệu vào file JSON
    fs.writeFileSync(LOG_FILE, JSON.stringify(logs, null, 2));
}

// Tạo server HTTP
const server = http.createServer((req, res) => {
    res.setHeader("Access-Control-Allow-Origin", "*");
    res.setHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.setHeader("Access-Control-Allow-Headers", "Content-Type");

    if (req.method === "OPTIONS") {
        res.writeHead(204);
        return res.end();
    }

    if (req.method === "POST") {
        let body = "";
        req.on("data", chunk => (body += chunk));
        req.on("end", () => {
            try {
                const jsonData = JSON.parse(body);
                saveLog(jsonData);
                res.writeHead(200, { "Content-Type": "text/plain" });
                res.end("Data saved");
            } catch (err) {
                res.writeHead(400, { "Content-Type": "text/plain" });
                res.end("Invalid JSON");
            }
        });
    } else if (req.method === "GET") {
        if (!fs.existsSync(LOG_FILE)) {
            res.writeHead(404, { "Content-Type": "text/plain" });
            return res.end("No logs found");
        }

        const logs = JSON.parse(fs.readFileSync(LOG_FILE, "utf8"));
        const urlParts = req.url.split("/").filter(part => part); // Loại bỏ các dấu "/" trống

        let result = logs;
        for (const part of urlParts) {
            if (result[part]) {
                result = result[part]; // Tiếp tục truy vấn sâu hơn
            } else {
                res.writeHead(404, { "Content-Type": "text/plain" });
                return res.end("Not found");
            }
        }

        res.writeHead(200, { "Content-Type": "application/json" });
        res.end(JSON.stringify(result, null, 2));
    } else {
        res.writeHead(405, { "Content-Type": "text/plain" });
        res.end("Method not allowed");
    }
});

// Khởi động server
server.listen(PORT, "0.0.0.0", () => {
    console.log(`Server is running at http://localhost:${PORT}`);
});
