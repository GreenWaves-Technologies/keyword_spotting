#include "AutoTilerLib.h"
#include "AutoTilerLibTypes.h"
#include "MfccGenerator.h"
#ifdef SMALL
  #include "MFCC_params_SMALL.h"
#endif
#ifdef MEDIUM
  #include "MFCC_params_MEDIUM.h"
#endif
#ifdef LARGE
  #include "MFCC_params_LARGE.h"
#endif
  

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
    MFCC_Generator("MFCC",        0, N_FRAME, FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_COEFF_CNT, N_DCT, PREEMP_FACTOR, LIFTER_COEFF, USE_RADIX_4, USE_POWER, DATA_TYPE, MFCC_BANK_CNT, 0, 0);
  //MFCC_Generator("MFCC_single", 0,       1, FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_COEFF_CNT, N_DCT, PREEMP_FACTOR, LIFTER_COEFF, USE_RADIX_4, USE_POWER, DATA_TYPE, MFCC_BANK_CNT, 0, 0);
    GenerateTilingCode();
}
