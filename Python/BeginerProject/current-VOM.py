import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

# Đọc dữ liệu từ tệp CSV
df = pd.read_csv('data-VOM.csv', header=None, names=['Time', 'Value1', 'Value2'])

# Chuyển đổi cột thời gian sang định dạng datetime
df['Time'] = pd.to_datetime(df['Time'], format='%H:%M:%S')

# Đặt font chữ
plt.rcParams['font.family'] = 'Times New Roman'  # Thay đổi thành tên font bạn muốn sử dụng
plt.rcParams['font.size'] = 12  # Thay đổi kích thước font chữ

# Vẽ biểu đồ
fig, ax = plt.subplots(figsize=(10, 5))
ax.plot(df['Time'], df['Value2'], linestyle='-', color='orange', label='Current from VOM')
ax.plot(df['Time'], df['Value1'], linestyle='--', color='blue', label='Current from sensor')

# Đặt định dạng cho trục x
ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

plt.title('Current value chart of experiment')
plt.xlabel('Time')
plt.ylabel('Current (mA)')
plt.legend()
plt.grid(True)
plt.show()
