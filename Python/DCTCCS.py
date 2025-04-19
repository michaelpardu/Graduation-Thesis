import numpy as np
import matplotlib.pyplot as plt
from skimage.metrics import structural_similarity as ssim

# ⚙️ Cấu hình
N = 500
compression_ratio = 0.3  # Giữ lại 10% hệ số đầu tiên

def load_lut_from_header(filename="dct_lut.h"):
    with open(filename, "r") as f:
        content = f.readlines()
    lut = []
    for line in content:
        line = line.strip()
        if line.startswith("const float DCT_LUT") or line.startswith("#") or line == "":
            continue
        line = line.replace("{", "").replace("}", "").replace(";", "").replace("f", "").strip()
        if line:
            values = [v for v in line.split(",") if v.strip()]  # Filter out empty strings
            lut.extend(map(float, values))
    size = int(len(lut) ** 0.5)  # Assuming LUT is square
    return np.array(lut, dtype=np.float32).reshape(size, size)

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

def normalize_data(data):
    """Normalize data to the range [0, 1]."""
    min_val = np.min(data)
    max_val = np.max(data)
    return (data - min_val) / (max_val - min_val), min_val, max_val

def denormalize_data(data, min_val, max_val):
    """Denormalize data back to its original range."""
    return data * (max_val - min_val) + min_val

if __name__ == "__main__":
    lut = load_lut_from_header()

    # 📥 Nhập dữ liệu từ người dùng
    data = np.array([1182,1188,1178,1172,1125,1099,1088,1051,1034,1018,995,945,959,899,901,919,880,809,813,818,854,831,802,801,813,839,900,903,896,911,926,944,932,962,961,970,934,945,976,983,995,959,967,977,963,959,976,986,1005,991,1011,1002,996,993,962,945,880,801,783,806,811,807,819,800,815,816,816,828,826,827,818,779,810,865,862,878,879,887,876,895,915,900,871,880,903,913,881,889,889,886,882,862,864,873,897,912,896,877,864,903,897,896,897,870,897,902,915,919,906,918,927,1046,1126,1121,1117,1136,1119,1139,1148,1166,1183,1188,1162,1183,1182,1187,1213,1210,1197,1217,1212,1201,1193,1168,1162,1178,1199,1163,1156,1163,1134,1117,1022,983,1019,1092,1110,1105,1101,1115,1094,1133,1109,1129,1132,1121,1109,1116,1101,1099,1104,1095,1101,1099,1100,1091,1107,1097,1129,1127,1123,1090,1084,1008,967,885,841,870,891,911,927,1037,1142,1287,1455,1622,1838,1965,2077,2138,2075,1786,1581,1415,1294,1193,1106,1040,964,948,954,928,944,925,900,880,857,881,862,886,865,907,898,934,923,915,891,897,923,912,924,925,915,931,960,927,878,811,852,895,931,922,958,963,959,1003,1003,989,1012,992,992,987,1011,994,1022,1023,1056,1040,1046,1011,1006,1009,1017,1005,1020,1002,1030,1039,1038,1035,1054,1089,1059,1075,1071,1066,1071,1059,1038,942,912,912,951,1008,1072,1102,1101,1123,1136,1154,1151,1143,1140,1150,1171,1184,1207,1232,1203,1168,1147,1136,1136,1150,1121,1119,1088,1101,1099,1107,1104,1104,1099,1101,1073,1050,1054,1003,1002,975,915,845,847,897,942,912,906,807,688,621,523,528,581,679,742,769,788,801,798,816,848,839,835,866,889,883,891,894,899,880,849,843,853,881,910,914,935,927,971,959,968,976,997,943,946,938,923,952,953,960,979,1002,990,976,978,976,982,977,977,1004,1009,1003,1014,994,1013,1008,978,1040,1086,1161,1181,1186,1215,1205,1238,1246,1254,1278,1275,1280,1275,1281,1234,1195,1119,1088,1047,1009,1009,989,965,959,939,892,940,953,986,982,976,1008,1007,983,977,994,976,1008,1042,1038,1015,1054,1043,1049,1033,1072,1072,1050,1071,1063,1088,1077,1046,1065,1035,1047,1031,1048,1059,1039,1031,1034,1031,1023,1059,1047,1041,1042,1059,1055,1073,1056,1071,1062,1077,1070,1072,1072,1046,1047,998,993,1035,1106,1187,1233,1219,1242,1215,1232,1240,1205,1169,1142,1087,1040,1067,1067,1059,1052,1049,1102,1217,1375,1666,1920,2045,2063,2165,2127,1861,1598,1416,1293,1227,1136,1074,1034,1042,1059,1033,1053], dtype=np.float32)

    # Normalize data
    data_normalized, min_val, max_val = normalize_data(data)

    # Perform DCT
    dct_data = dct_using_lut(data_normalized, lut)
    print("Dữ liệu miền tần số sau DCT:")
    print(dct_data)
    # Compression
    keep = int(len(data) * compression_ratio)
    dct_data_compressed = np.copy(dct_data)
    dct_data_compressed[keep:] = 0

    # Reconstruct and denormalize
    reconstructed_normalized = idct_using_lut(dct_data_compressed, lut)
    reconstructed = denormalize_data(reconstructed_normalized, min_val, max_val)

    # Compute metrics
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
