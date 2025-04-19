import React, { useEffect, useState } from "react";
import Slider from "rc-slider";
import "rc-slider/assets/index.css"; // Ensure the slider styles are imported

export default function RangeControls({
  showValue,
  handleSlider1Change,
  visibleRange,
  handleSlider2Change,
  filteredData,
  handleJumpToStart,
  handlePrev,
  handleNext,
  handleJumpToEnd,
  isLiveView,
  handleExportData,
}) {
  const [sliderValue, setSliderValue] = useState(showValue);
  const [startTime, setStartTime] = useState(visibleRange[0]);

  useEffect(() => {
    setSliderValue(showValue); // Reset slider value when showValue changes
    setStartTime(visibleRange[0]); // Reset start time when visibleRange changes
  }, [showValue, visibleRange]);

  return (
    <div>
      {/* Slider for adjusting the visible range */}
      <div className="mt-4">
        <label className="block mb-2">Adjust Visible Range:</label>
        <Slider
          min={1}
          max={Math.min(20000, filteredData.length)} // Ensure max value doesn't exceed data length
          value={sliderValue}
          onChange={(value) => {
            setSliderValue(value);
            handleSlider1Change(value);
          }}
          trackStyle={[{ backgroundColor: "#8884d8" }]}
          handleStyle={[{ borderColor: "#8884d8" }]}
        />
        <div className="mt-2 flex justify-between text-sm">
          <span>Show Value: {sliderValue}</span>
        </div>
      </div>

      {/* Slider for adjusting the start time */}
      {!isLiveView && (
        <>
          <div className="mt-4">
            <label className="block mb-2">Adjust Start Time:</label>
            <Slider
              min={0}
              max={Math.max(0, filteredData.length - sliderValue)} // Ensure max value is valid
              value={startTime}
              onChange={(value) => {
                setStartTime(value);
                handleSlider2Change(value);
              }}
              trackStyle={[{ backgroundColor: "#82ca9d" }]}
              handleStyle={[{ borderColor: "#82ca9d" }]}
            />
            <div className="mt-2 flex justify-between text-sm">
              <span>Start Sample: {startTime + 1}</span>
              <span> - </span>
              <span>End Sample: {startTime + sliderValue}</span>
            </div>
          </div>

          {/* Navigation buttons */}
          <div className="mt-4 flex flex-wrap gap-2">
            <button onClick={handleJumpToStart} className="button-primary w-full sm:w-auto">Jump to Start</button>
            <button onClick={handlePrev} className="button-primary w-full sm:w-auto">Prev {sliderValue}</button>
            <button onClick={handleNext} className="button-primary w-full sm:w-auto">Next {sliderValue}</button>
            <button onClick={handleJumpToEnd} className="button-primary w-full sm:w-auto">Jump to End</button>
          </div>

          {/* Export data button */}
          <div className="mt-4">
            <button onClick={handleExportData} className="button-primary w-full sm:w-auto">
              Export Data as CSV
            </button>
          </div>
        </>
      )}
    </div>
  );
}
