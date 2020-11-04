#include "Gap.h"
#include "MfccBasicKernels.h"

void LoadMFCCLibrary();
void IMFCC_parallel(char *Name, int nbin, int n_fft,  int imfcc_coeff_cnt);
void MFCC_parallel(char *Name, int FrameSize, int n_fft, int numcep, int mfcc_coeff_cnt, int do_dct, int do_lifter, int n_dct);
