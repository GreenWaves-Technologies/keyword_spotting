#include "Gap.h"
#include "MfccBasicKernels.h"

void LoadMFCCLibrary();
void IMFCC_parallel(char *Name, int nbin, int n_fft,  int imfcc_coeff_cnt);
void MFCC_parallel(char *Name, int FrameSize, int n_fft, int numcep, int mfcc_coeff_cnt, int do_dct, int do_lifter, int fft_radix_type, int n_dct, int MFCCFromPower, float PreempFactor);
void MFCC2D_Generator(char *Name, CNN_GenControl_T *Ctrl,	int NFrames, int FrameSize,	int FrameStride, int Nfft, int NMFCCCoeff, int Ndct, float PreempFactor, int do_dct, int do_lifter, int high_prec_fft);