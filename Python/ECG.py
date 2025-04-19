import pandas as pd # đọc file csv
import matplotlib.pyplot as plt # vẽ biểu đồ

# Đọc dữ liệu từ tệp CSV
df = pd.read_csv('file1.csv', header=None, names=['ADC Value'])

# Điện áp tham chiếu
Vref = 3.3

# Chuyển đổi giá trị ADC sang millivolt (mV)
df['Voltage (mV)'] = (df['ADC Value'] / 4095) * Vref * 1000

# Số lượng mẫu cố định
num_samples = 100000

# Lấy 500 mẫu đầu tiên
values = df['Voltage (mV)'][:num_samples].tolist()

# Tạo danh sách thời gian tương ứng (2ms giữa các mẫu)
times = [i * 2 for i in range(len(values))]

# Vẽ biểu đồ
plt.figure(figsize=(10, 5))
plt.plot(times, values, linestyle='-', color='blue', label='Voltage (mV)')
plt.title('Biểu đồ tín hiệu ECG của ' + 'Trần Quốc Duy - 29/10/2024', fontsize=14)
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
plt.gca().yaxis.grid(True, which='both', linestyle='-', linewidth=0.5)

# Hiển thị số chỉ mỗi 200ms trên trục x
plt.gca().xaxis.set_tick_params(which='major', length=10, width=2)
plt.gca().xaxis.set_tick_params(which='minor', length=5, width=1)

plt.show()
