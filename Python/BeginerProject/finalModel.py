import tensorflow as tf
import numpy as np

# Tải mô hình TensorFlow Lite đã lưu
interpreter = tf.lite.Interpreter(model_path="model.tflite")
interpreter.allocate_tensors()

# Lấy thông tin về tensor
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# Hàm dự đoán với dữ liệu mới
def predict(input_value):
    input_data = np.array([[input_value]], dtype=np.float32)
    interpreter.set_tensor(input_details[0]['index'], input_data)
    interpreter.invoke()
    output_data = interpreter.get_tensor(output_details[0]['index'])
    return output_data[0][0] > 0.5  # Trả về True nếu xác suất > 0.5, ngược lại False

# Kiểm tra toàn bộ giá trị từ 0 đến 100
correct_predictions = 0
for value in range(5000):
    result = predict(value)
    expected = value >= 50  # Kỳ vọng giá trị > 50 là Đúng, ngược lại Sai
    if result == expected:
        correct_predictions += 1
    print(f"Giá trị {value}: {'Đúng' if result else 'Sai'} (Kỳ vọng: {'Đúng' if expected else 'Sai'})")

accuracy = correct_predictions / 5000 * 100
print(f"Độ chính xác: {accuracy:.2f}%")
