import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Cấu hình cổng Serial
ser = serial.Serial('COM3', 115200, timeout=1)

# Hàm cập nhật dữ liệu và vẽ biểu đồ
def update(frame):
    line = ser.readline().decode('utf-8').strip()
    if line:
        try:
            data = [(int(value) - 16384) for value in line.split(',') if 0 <= int(value) <= 30000]
            if data:
                plt.clf()  # Xóa biểu đồ cũ
                plt.plot(data, marker='', linestyle='-', color='b')
                plt.title('Biểu đồ Dữ liệu')
                plt.xlabel('Chỉ số')
                plt.ylabel('Giá trị')
                plt.grid(True)
        except ValueError:
            print("Received non-integer data, skipping")

# Thiết lập đồ họa động với tùy chọn `cache_frame_data=False`
fig = plt.figure()
ani = animation.FuncAnimation(fig, update, interval=1000, cache_frame_data=False)

plt.show()

# Đóng cổng Serial khi kết thúc
ser.close()
