import pandas as pd
import numpy as np

# Tạo dữ liệu giả lập
np.random.seed(42)  # Để tái tạo dữ liệu
time_stamps = pd.date_range(start="2023-01-01", periods=1500, freq='H')  # 1500 giờ liên tiếp
water_levels = np.random.uniform(low=25, high=28, size=(1500,))  # Mực nước dao động từ 10mm đến 30mm

# Tạo dataframe
data = {
    "time": time_stamps,
    "water_level": water_levels
}

df = pd.DataFrame(data)

# Lưu dữ liệu vào file CSV
df.to_csv("river_level_data.csv", index=False)

print("Data saved to 'river_level_data.csv'")

# Hiển thị một phần của dữ liệu
print(df.head())
