import pandas as pd
import matplotlib.pyplot as plt
from scipy.signal import butter, filtfilt, find_peaks

# Đọc dữ liệu từ tệp CSV
df = pd.read_csv('ecg tran duy 19092024 1st.csv', header=None, names=['ADC Value'])

# Điện áp tham chiếu
Vref = 3.3

# Chuyển đổi giá trị ADC sang millivolt (mV)
df['Voltage (mV)'] = (df['ADC Value'] / 4095) * Vref * 1000

# Số lượng mẫu cố định
num_samples = 1200

# Lấy 500 mẫu đầu tiên
values = df['Voltage (mV)'][:num_samples].tolist()

# Tạo danh sách thời gian tương ứng (2ms giữa các mẫu)
times = [i * 2 for i in range(len(values))]

# Thiết lập bộ lọc thông thấp Butterworth
def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

# Áp dụng bộ lọc thông thấp
def lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = filtfilt(b, a, data)
    return y

# Các tham số của bộ lọc
cutoff = 50  # Tần số cắt (Hz)
fs = 500  # Tần số lấy mẫu (500 Hz tương đương với mỗi mẫu cách nhau 2ms)

# Áp dụng bộ lọc thông thấp cho dữ liệu
filtered_values = lowpass_filter(values, cutoff, fs)

# Tìm điểm peak bất thường nhất
peaks, _ = find_peaks(filtered_values, height=0)
if peaks.size > 0:
    highest_peak = peaks[filtered_values[peaks].argmax()]
else:
    highest_peak = None

# Vẽ biểu đồ
plt.figure(figsize=(10, 5))
plt.plot(times, filtered_values, linestyle='-', color='blue', label='Filtered Voltage (mV)')
if highest_peak is not None:
    plt.plot(times[highest_peak], filtered_values[highest_peak], "x", color='red', label='Highest Peak')

plt.title('Biểu đồ giá trị theo thời gian', fontsize=14)
plt.suptitle('Dữ liệu đo từ ESP32 với nguồn 3.3V', fontsize=10)
plt.xlabel('Thời gian (ms)')
plt.ylabel('Điện áp (mV)')
plt.legend()
plt.grid(True)

# Thêm các đường grid mỗi 40ms trên trục thời gian x
plt.gca().xaxis.set_minor_locator(plt.MultipleLocator(40))
plt.gca().xaxis.set_major_locator(plt.MultipleLocator(200))
plt.gca().grid(which='minor', linestyle=':', linewidth=0.5, color='lightgray')
plt.gca().grid(which='major', linestyle='-', linewidth=2, color='gray')

# Đảm bảo đường kẻ ngang trên trục y nhỏ như bình thường
plt.gca().yaxis.grid(True, which='both', linestyle='--', linewidth=0.5)

# Hiển thị số chỉ mỗi 200ms trên trục x
plt.gca().xaxis.set_tick_params(which='major', length=10, width=2)
plt.gca().xaxis.set_tick_params(which='minor', length=5, width=1)
plt.gca().set_xticklabels([str(i) for i in range(0, times[-1] + 1, 200)])

plt.show()
