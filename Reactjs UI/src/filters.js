// export function applyKalmanFilter(data, R, Q, P0, K) {
//     let P = P0;
//     let x = data[0]?.value || 0;
//     let filteredData = data.map((point) => {
//       P = P + Q;
//       K = P / (P + R);
//       x = x + K * (point.value - x);
//       P = (1 - K) * P;
//       return { ...point, value: x };
//     });
//     return filteredData;
//   }
  
//   export function detectRPeaks(data) {
//     let peaks = [];
//     let threshold = 0.6 * Math.max(...data.map(d => d.value)); // Ngưỡng ban đầu
    
//     for (let i = 1; i < data.length - 1; i++) {
//       if (data[i].value > data[i - 1].value && data[i].value > data[i + 1].value && data[i].value > threshold) {
//         peaks.push({ ...data[i], value: data[i].value });
//       }
//     }
//     return peaks;
//   }
  