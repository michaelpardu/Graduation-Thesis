import matplotlib.pyplot as plt
import csv

# Đọc dữ liệu từ file sample.csv
data_list = []
with open('file1.csv', 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        for value in row:   
            data_list.append(int(value))

# Vẽ biểu đồ
plt.figure(figsize=(10, 6))
plt.plot(data_list, marker='o', linestyle='-', color='b')
plt.title('Biểu đồ Dữ liệu')
plt.xlabel('Chỉ số')
plt.ylabel('Giá trị')
plt.grid(True)
plt.show()
