import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy.fftpack import dct, idct

# Đọc dữ liệu từ file CSV
def read_ecg_from_csv(file_path):
    df = pd.read_csv(file_path)
    return df.iloc[:1000, 0].values  # Chỉ lấy 500 số liệu đầu tiên

# Thay đổi tên file CSV tại đây
ecg_signal = read_ecg_from_csv("ecg tran duy 19092024 1st.csv")

# Thực hiện DCT
dct_coeffs = dct(ecg_signal, norm='ortho')

# Xuất hệ số DCT ra file CSV
np.savetxt("dct_coeffs.csv", dct_coeffs, delimiter=",")
print("Hệ số DCT đã được lưu vào file 'dct_coeffs.csv'")

# Đọc lại hệ số DCT từ file CSV
loaded_dct_coeffs = np.loadtxt("dct_coeffs.csv", delimiter=",")
print("Hệ số DCT đã được đọc lại từ file 'dct_coeffs.csv'")

# Giải mã tín hiệu bằng IDCT
reconstructed_signal_from_file = idct(loaded_dct_coeffs, norm='ortho')

# Tính dung lượng gốc (mỗi giá trị là float64 -> 8 byte)
original_size = len(ecg_signal) * 8
print(f"Dung lượng tín hiệu gốc: {original_size} bytes")

# Chọn tỷ lệ giữ lại (10%, 20%, 50% hệ số DCT)
compression_ratios = [0.1, 0.2, 0.5]
reconstructed_signals = {}

for ratio in compression_ratios:
    num_coeffs = int(len(dct_coeffs) * ratio)  # Số hệ số giữ lại
    dct_compressed = np.zeros_like(dct_coeffs)
    dct_compressed[:num_coeffs] = dct_coeffs[:num_coeffs]  # Chỉ giữ lại các hệ số đầu

    # Tính dung lượng sau nén
    compressed_size = num_coeffs * 8
    print(f"Dung lượng sau khi nén {int(ratio*100)}% hệ số: {compressed_size} bytes")

    # Khôi phục tín hiệu bằng IDCT
    reconstructed_signals[ratio] = idct(dct_compressed, norm='ortho')

# Vẽ 4 biểu đồ cùng lúc
fig, axes = plt.subplots(2, 2, figsize=(12, 10))  # Tạo lưới 2x2

# Biểu đồ tín hiệu gốc
axes[0, 0].plot(ecg_signal, label="Tín hiệu gốc", linewidth=2, color='black')
axes[0, 0].set_title("Tín hiệu gốc")
axes[0, 0].set_xlabel("Mẫu")
axes[0, 0].set_ylabel("Biên độ")
axes[0, 0].legend()
axes[0, 0].grid()

# Biểu đồ riêng biệt cho từng tỷ lệ nén
for idx, (ratio, rec_signal) in enumerate(reconstructed_signals.items()):
    ax = axes[(idx + 1) // 2, (idx + 1) % 2]  # Xác định vị trí trong lưới
    ax.plot(ecg_signal, label="Tín hiệu gốc", linewidth=2, color='black', alpha=0.3)  # Giảm độ mờ xuống 30%
    ax.plot(rec_signal, linestyle='--', label=f"Nén {int(ratio*100)}% hệ số", color='red')
    ax.set_title(f"Nén {int(ratio*100)}% hệ số")
    ax.set_xlabel("Mẫu")
    ax.set_ylabel("Biên độ")
    ax.legend()
    ax.grid()

# Điều chỉnh khoảng cách giữa các biểu đồ
plt.tight_layout()
plt.show()

# Hiển thị tín hiệu gốc và tín hiệu được giải mã từ file
plt.figure(figsize=(10, 5))
plt.plot(ecg_signal, label="Tín hiệu gốc", linewidth=2, color='black')
plt.plot(reconstructed_signal_from_file, linestyle='--', label="Tín hiệu giải mã từ file", color='red')
plt.title("So sánh tín hiệu gốc và tín hiệu giải mã từ file")
plt.xlabel("Mẫu")
plt.ylabel("Biên độ")
plt.legend()
plt.grid()
plt.show()