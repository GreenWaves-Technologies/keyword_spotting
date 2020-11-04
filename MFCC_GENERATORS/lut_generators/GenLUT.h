
typedef enum {
  HAMMING_FN,
  HANNING_FN
} fn_window_t;


void BuildLUT(char *OutputDirName, int FrameSize, int NumFFT, int NumDCT, int NumCEP, int DoDCT, int FFTRadixType, int L, fn_window_t Window_fn);
void BuildLUT_bin(char *OutputDirName, int FrameSize, int NumFFT, int NumDCT, int NumCEP, int DoDCT, int FFTRadixType, int L);
void BuildFB(char *OutputDirName, int SampleRate, int N_fft, int Nbanks, int Fmin, int Fmax, int mfcc_coeff_dyn);

void SetupTwiddlesLUT(signed short *Twiddles, int Nfft, int Inverse);
void SetupR4SwapTable (short int *SwapTable, int Ni);
void SetupR2SwapTable (short int *SwapTable, int Ni);
void SetupTwidend(short int *twidend, int Ni);
void SetupLiftCoeff(short int *__restrict__ LiftCoeff,  int FrameSize, int L);
void SetupHammingWindow(short int * __restrict__ HammingLUT, int FrameSize);
void SetupHanningWindow(short int * __restrict__ HanningLUT, int FrameSize);
void SetupDCTTable(short int *dct_coeff, int N_dct);

#define FP2FIX(Val, Precision)                 ((int)((Val)*((1 << (Precision))-1)))
#define FP2FIXR(Val, Precision)                ((int)((Val)*((1 << (Precision))-1) + 0.5))

#define Q15	15
#define FFT_TWIDDLE_DYN 15
#define gap_fl1(x)                     (31 - __builtin_clz((x)))
