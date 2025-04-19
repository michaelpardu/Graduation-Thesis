import pandas as pd
import matplotlib.pyplot as plt

# Danh sách các tệp CSV cụ thể
csv_files = ["file1.csv", "file2.csv", "file3.csv", "file4.csv"]

plt.figure(figsize=(10, 6))

# Duyệt qua 4 tệp CSV
for file in csv_files:
    df = pd.read_csv(file)
    
    # Giả sử cột đầu tiên chứa dữ liệu cần vẽ
    column_name = df.columns[0]  # Lấy tên cột đầu tiên
    data = df[column_name][:5000]  # Lấy 5000 số liệu đầu tiên
    
    plt.plot(data, label=file)

plt.xlabel("Sample Index")
plt.ylabel("Value")
plt.title("Biểu đồ dữ liệu từ 4 file CSV")
plt.legend()
plt.show()