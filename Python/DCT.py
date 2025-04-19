import numpy as np
import matplotlib.pyplot as plt
from skimage.metrics import structural_similarity as ssim

# ⚙️ Cấu hình
N = 500
compression_ratio = 0.1  # Giữ lại 10% hệ số đầu tiên

def load_lut_from_header(filename="dct_lut.h"):
    with open(filename, "r") as f:
        content = f.read()
    start = content.find("{")
    end = content.rfind("}")
    array_text = content[start:end+1]

    array_text = array_text.replace("f", "")
    array_text = array_text.replace("{", "[")
    array_text = array_text.replace("}", "]")
    lut = eval(array_text)
    return np.array(lut, dtype=np.float32)

def dct_using_lut(data, lut):
    return np.dot(lut, data)

def idct_using_lut(dct_data, lut):
    return np.dot(lut.T, dct_data)

def compute_metrics(original, reconstructed):
    mse = np.mean((original - reconstructed) ** 2)
    max_val = np.max(original)
    psnr = 10 * np.log10((max_val ** 2) / mse) if mse != 0 else np.inf
    ssim_val = ssim(original, reconstructed, data_range=original.max() - original.min())
    return mse, psnr, ssim_val

if __name__ == "__main__":
    lut = load_lut_from_header()

    # 📊 Dữ liệu mẫu: sóng sin chuẩn hóa 0–4095
    x = np.linspace(0, 2 * np.pi, N)
    data = ((np.sin(x) + 1) / 2 * 4095).astype(np.float32)

    dct_data = dct_using_lut(data, lut)

    keep = int(N * compression_ratio)
    dct_data_compressed = np.copy(dct_data)
    dct_data_compressed[keep:] = 0

    reconstructed = idct_using_lut(dct_data_compressed, lut)

    mse, psnr, ssim_val = compute_metrics(data, reconstructed)

    # 📈 Vẽ biểu đồ
    fig, axs = plt.subplots(2, 2, figsize=(12, 8))

    # Biên dạng gốc và tái tạo
    axs[0, 0].plot(data, label='Gốc')
    axs[0, 0].plot(reconstructed, label='Tái tạo', linestyle='--')
    axs[0, 0].set_title(f"Miền thời gian\nMSE={mse:.2f}, PSNR={psnr:.2f} dB, SSIM={ssim_val:.4f}")
    axs[0, 0].legend()

    # Hệ số DCT gốc
    axs[0, 1].stem(dct_data, linefmt='C1-', markerfmt='C1o', basefmt=" ")
    axs[0, 1].set_title("Hệ số DCT (trước nén)")

    # Hệ số DCT sau nén
    axs[1, 0].stem(dct_data_compressed, linefmt='C2-', markerfmt='C2o', basefmt=" ")
    axs[1, 0].set_title(f"Hệ số DCT (sau khi giữ {compression_ratio*100:.0f}%)")

    # Độ sai lệch
    axs[1, 1].plot(data - reconstructed, color='red')
    axs[1, 1].set_title("Sai lệch (Original - Reconstructed)")

    plt.tight_layout()
    plt.show()
