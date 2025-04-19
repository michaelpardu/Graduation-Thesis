#include "DCTLibrary.h"

void DCTLibrary::compute_dct(float* input, float* output, int size) {
    for (int k = 0; k < size; ++k) {
        output[k] = 0;
        for (int n = 0; n < size; ++n) {
            output[k] += input[n] * DCT_LUT[k][n];
        }
    }
}
