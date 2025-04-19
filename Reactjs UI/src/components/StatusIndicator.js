import React from "react";

export default function StatusIndicator({ isLiveView }) {
  return (
    <div className="mb-4">
      <span className={`status-indicator ${isLiveView ? "status-live" : "status-replay"}`}>
        {isLiveView ? "Live View •" : "Replay View 🕮"}
      </span>
    </div>
  );
}
