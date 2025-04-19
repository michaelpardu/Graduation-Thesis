import React from "react";

export default function DatePicker({ selectedDate, setSelectedDate }) {
  return (
    <div className="mb-4">
      <label htmlFor="date-picker" className="block mb-2">Select Date:</label>
      <input
        type="date"
        id="date-picker"
        value={selectedDate}
        onChange={(e) => setSelectedDate(e.target.value)}
        className="p-2 border rounded w-full sm:w-auto"
      />
    </div>
  );
}
