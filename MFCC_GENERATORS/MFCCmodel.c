#include "AutoTilerLib.h"
#include "AutoTilerLibTypes.h"
#include "MFCC_Generator.h"
#include "MFCC_params.h"

#define RADIX2 0
#define RADIX4 1

void MFCCConfiguration(unsigned int L1Memory)
{
  SetInlineMode(ALWAYS_INLINE);
	SetSymbolDynamics();

  SetUsedFilesNames(0, 1, "MfccBasicKernels.h");
  SetGeneratedFilesNames("MFCCKernels.c", "MFCCKernels.h");

  SetL1MemorySize(L1Memory);
}

int main(int argc, char **argv)
{
  	if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");

    // Set Auto Tiler configuration, given shared L1 memory is 51200
    MFCCConfiguration(51200);
    // Load FIR basic kernels
    LoadMFCCLibrary();
    MFCC2D_Generator("MFCC", 0, N_FRAME, FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_COEFF_CNT, N_DCT, 0.0, 1, 0, 0);
    MFCC2D_Generator("MFCC00", 0, 1, FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_COEFF_CNT, N_DCT, 0.0, 1, 0, 0);
    GenerateTilingCode();
}
