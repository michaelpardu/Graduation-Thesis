import React, { useEffect } from "react";

export default function Header({ isDarkMode, toggleDarkMode, isLiveView, setIsLiveView }) {
  useEffect(() => {
    // Update the body class based on the dark mode state
    document.body.classList.toggle("dark-mode", isDarkMode);
  }, [isDarkMode]);

  return (
    <div>
      <div
        style={{
          position: "fixed",
          top: "50%",
          right: -10,
          zIndex: 1000,
        }}
      >
        <button onClick={toggleDarkMode} className="button-primary p-2">
          {isDarkMode ? "☀️" : "🌙"}
        </button>
      </div>
      <div className="flex flex-wrap gap-2 mb-4">
        <button
          onClick={() => setIsLiveView(true)}
          className={`button-primary ${isLiveView ? "bg-blue-500" : "bg-gray-300"}`}
        >
          Live View
        </button>
        <button
          onClick={() => setIsLiveView(false)}
          className={`button-primary ${!isLiveView ? "bg-blue-500" : "bg-gray-300"}`}
        >
          Replay View
        </button>
      </div>
    </div>
  );
}
