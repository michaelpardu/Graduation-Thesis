import numpy as np
import matplotlib.pyplot as plt
from scipy.fftpack import dct, idct
from skimage.metrics import structural_similarity as ssim

# ⚙️ Cấu hình
N = 500
compression_ratio = 0.1  # Giữ lại 10% hệ số đầu tiên

def compute_metrics(original, reconstructed):
    mse = np.mean((original - reconstructed) ** 2)
    max_val = np.max(original)
    psnr = 10 * np.log10((max_val ** 2) / mse) if mse != 0 else np.inf
    ssim_val = ssim(original, reconstructed, data_range=original.max() - original.min())
    return mse, psnr, ssim_val

if __name__ == "__main__":
    # 📊 Dữ liệu mẫu: sóng sin chuẩn hóa 0–4095
    x = np.linspace(0, 2 * np.pi, N)
    data = ((np.sin(x) + 1) / 2 * 4095).astype(np.float32)

    # === 1. DCT lần 1 ===
    dct_data_1 = dct(data, type=3, norm='ortho')
    keep_1 = int(N * compression_ratio)
    dct_data_1_compressed = np.copy(dct_data_1)
    dct_data_1_compressed[keep_1:] = 0
    reconstructed_1 = idct(dct_data_1_compressed, type=3, norm='ortho')

    # === 2. DCT lần 2 ===
    dct_data_2 = dct(reconstructed_1, type=3, norm='ortho')
    keep_2 = int(N * compression_ratio)
    dct_data_2_compressed = np.copy(dct_data_2)
    dct_data_2_compressed[keep_2:] = 0
    reconstructed_2 = idct(dct_data_2_compressed, type=3, norm='ortho')

    # === 3. DCT lần 3 ===
    dct_data_3 = dct(reconstructed_2, type=3, norm='ortho')
    keep_3 = int(N * compression_ratio)
    dct_data_3_compressed = np.copy(dct_data_3)
    dct_data_3_compressed[keep_3:] = 0
    reconstructed_3 = idct(dct_data_3_compressed, type=3, norm='ortho')

    # === Tính toán các chỉ số ===
    mse_1, psnr_1, ssim_1 = compute_metrics(data, reconstructed_1)
    mse_2, psnr_2, ssim_2 = compute_metrics(data, reconstructed_2)
    mse_3, psnr_3, ssim_3 = compute_metrics(data, reconstructed_3)

    # === Vẽ biểu đồ ===
    fig, axs = plt.subplots(3, 3, figsize=(15, 12))

    # Lần 1
    axs[0, 0].plot(data, label='Gốc')
    axs[0, 0].plot(reconstructed_1, label='Tái tạo', linestyle='--')
    axs[0, 0].set_title(f"Lần 1: MSE={mse_1:.2f}, PSNR={psnr_1:.2f} dB, SSIM={ssim_1:.4f}")
    axs[0, 0].legend()
    axs[0, 1].stem(dct_data_1, linefmt='C1-', markerfmt='C1o', basefmt=" ")
    axs[0, 1].set_title("Hệ số DCT (Lần 1)")
    axs[0, 2].stem(dct_data_1_compressed, linefmt='C2-', markerfmt='C2o', basefmt=" ")
    axs[0, 2].set_title("Hệ số DCT (Lần 1, sau nén)")

    # Lần 2
    axs[1, 0].plot(data, label='Gốc')
    axs[1, 0].plot(reconstructed_2, label='Tái tạo', linestyle='--')
    axs[1, 0].set_title(f"Lần 2: MSE={mse_2:.2f}, PSNR={psnr_2:.2f} dB, SSIM={ssim_2:.4f}")
    axs[1, 0].legend()
    axs[1, 1].stem(dct_data_2, linefmt='C1-', markerfmt='C1o', basefmt=" ")
    axs[1, 1].set_title("Hệ số DCT (Lần 2)")
    axs[1, 2].stem(dct_data_2_compressed, linefmt='C2-', markerfmt='C2o', basefmt=" ")
    axs[1, 2].set_title("Hệ số DCT (Lần 2, sau nén)")

    # Lần 3
    axs[2, 0].plot(data, label='Gốc')
    axs[2, 0].plot(reconstructed_3, label='Tái tạo', linestyle='--')
    axs[2, 0].set_title(f"Lần 3: MSE={mse_3:.2f}, PSNR={psnr_3:.2f} dB, SSIM={ssim_3:.4f}")
    axs[2, 0].legend()
    axs[2, 1].stem(dct_data_3, linefmt='C1-', markerfmt='C1o', basefmt=" ")
    axs[2, 1].set_title("Hệ số DCT (Lần 3)")
    axs[2, 2].stem(dct_data_3_compressed, linefmt='C2-', markerfmt='C2o', basefmt=" ")
    axs[2, 2].set_title("Hệ số DCT (Lần 3, sau nén)")

    plt.tight_layout()
    plt.show()
