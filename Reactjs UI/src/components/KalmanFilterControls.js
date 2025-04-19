import React from "react";

export default function KalmanFilterControls({ kalmanParams, setKalmanParams, handleApplyFilter }) {
  return (
    <div>
      <h2 className="text-lg font-semibold">Kalman Filter Parameters:</h2>
      <div className="mt-4 flex flex-wrap gap-2 items-center">
        <label className="w-full sm:w-auto">Q: </label>
        <input type="number" step="0.1" value={kalmanParams.q} onChange={(e) => setKalmanParams({ ...kalmanParams, q: Number(e.target.value) })} className="p-2 border rounded w-full sm:w-auto" />
        <label className="w-full sm:w-auto">R: </label>
        <input type="number" step="0.1" value={kalmanParams.r} onChange={(e) => setKalmanParams({ ...kalmanParams, r: Number(e.target.value) })} className="p-2 border rounded w-full sm:w-auto" />
        <label className="w-full sm:w-auto">P: </label>
        <input type="number" step="0.1" value={kalmanParams.p} onChange={(e) => setKalmanParams({ ...kalmanParams, p: Number(e.target.value) })} className="p-2 border rounded w-full sm:w-auto" />
        <label className="w-full sm:w-auto">K: </label>
        <input type="number" step="0.1" value={kalmanParams.k} onChange={(e) => setKalmanParams({ ...kalmanParams, k: Number(e.target.value) })} className="p-2 border rounded w-full sm:w-auto" />
        <button onClick={handleApplyFilter} className="button-primary w-full sm:w-auto">Apply Filter</button>
      </div>
    </div>
  );
}
