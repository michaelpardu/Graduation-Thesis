import numpy as np

N = 500
DCT_LUT = np.zeros((N, N), dtype=np.float32)

for k in range(N):
    for n in range(N):
        DCT_LUT[k, n] = np.cos(np.pi * (n + 0.5) * k / N)

# Chuẩn hóa
DCT_LUT *= np.sqrt(2 / N)
DCT_LUT[0, :] *= 1 / np.sqrt(2)

# Xuất sang định dạng C header
with open("dct_lut.h", "w") as f:
    f.write("const float DCT_LUT[500][500] = {\n")
    for row in DCT_LUT:
        f.write("    {" + ",".join(f"{val:.8f}" for val in row) + "},\n")
    f.write("};\n")
