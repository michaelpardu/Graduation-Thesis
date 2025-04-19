#ifndef DCTLIBRARY_H
#define DCTLIBRARY_H

#include "dct_lut.h"

class DCTLibrary {
public:
    static void compute_dct(float* input, float* output, int size);
};

#endif
