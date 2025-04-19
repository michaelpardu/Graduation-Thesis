import React from "react";
import { LineChart, Line, XAxis, YAxis, Tooltip, CartesianGrid, ResponsiveContainer } from "recharts";

export default function Chart({ data, visibleRange, showValue, handleScroll }) {
  return (
    <div className="h-[400px] overflow-y-auto" onWheel={handleScroll}>
      <ResponsiveContainer width="100%" height={400}>
        <LineChart data={data.slice(visibleRange[0], visibleRange[1])}>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis
            dataKey="time" // Use 'time' (sample index) for X-axis labels
            interval={Math.max(1, Math.floor(showValue / 10))}
            angle={-30}
            height={60}
            tick={{ fontSize: 10 }}
          />
          <YAxis />
          <Tooltip />
          <Line type="monotone" dataKey="value" stroke="#8884d8" dot={false} />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
}
