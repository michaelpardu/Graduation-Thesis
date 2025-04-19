import React from "react";

export default function AppHeader({ UID, onLogout }) {
  return (
    <header className="app-header">
        <span>Ξ PLC & IIoT Lab</span>
        <span>UID: {UID}</span>
        <span>
        <button onClick={onLogout} className="button-primary p-2">
          Logout
        </button>
        </span>
    </header>
  );
}
