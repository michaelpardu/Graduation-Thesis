import React, { useState } from "react";
import logo from "../eds-logo.svg"

export default function Login({ onLogin }) {
  const [uid, setUid] = useState("");
  const [apiKey, setApiKey] = useState("");
  const [error, setError] = useState("");

  const handleLogin = (e) => {
    e.preventDefault();

    // Validate UID and API key
    if (uid === "EDS" && apiKey === "IzUKf0FyBbktDiRxgry6fpRg8NTpxmb8XU777DDhwqDVnuUbolhSYxSUsijwBkN2") {
      onLogin({ UID: uid, apiKey });
    } else {
      setError("Invalid UID or API key");
    }
  };

  return (
    <div className="login-container">
      <div className="login-content">
        <div className="logo-container">
          <img src={logo} alt="EDS Logo" className="logo" />
          <h1 className="welcome-text">Welcome to 1-Lead ECG by EDS</h1>
          <p className="subtitle">How is your day going?</p>
        </div>
        <form onSubmit={handleLogin} className="login-form">
          <h2 className="text-2xl font-bold mb-4 text-center">Login</h2>
          {error && <p className="error-message">{error}</p>}
          <div>
            <label htmlFor="uid">UID</label>
            <input
              type="text"
              id="uid"
              value={uid}
              onChange={(e) => setUid(e.target.value)}
              required
            />
          </div>
          <div>
            <label htmlFor="apiKey">API Key</label>
            <input
              type="password"
              id="apiKey"
              value={apiKey}
              onChange={(e) => setApiKey(e.target.value)}
              required
            />
          </div>
          <button type="submit" className="button-primary">
            Login
          </button>
        </form>
      </div>
    </div>
  );
}
