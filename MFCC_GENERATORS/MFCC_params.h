
#define SAMPLERATE      16000 // sample_rate

#if defined(SMALL) || defined(MEDIUM)
	#define FRAME_SIZE      640   // 40ms - windows_size_samples
	#define FRAME_STEP	    320   // 20ms - window_stride_samples
	#define N_FRAME         49    // spectrogram_length (= 1 + int(length_minus_window / window_stride_samples))
	#define N_FFT           1024  // first fft points
	#define NUMCEP          10	  // How many output to take, usually = number of mfcc filter banks
	#define N_DCT           10    // number of DCT coeff
#elif defined(LARGE)
	#define FRAME_SIZE      480   // 40ms - windows_size_samples
	#define FRAME_STEP	    160   // 20ms - window_stride_samples
	#define N_FRAME         98    // spectrogram_length (= 1 + int(length_minus_window / window_stride_samples))
	#define N_FFT           512   // first fft points
	#define NUMCEP          40	  // How many output to take, usually = number of mfcc filter banks
	#define N_DCT           40    // number of DCT coeff
#endif

#define MFCC_COEFF_DYN  10	  // mfcc coeffs dynamic
#define MFCC_COEFF_CNT	495
#define FMIN            20	  // min freq (Hz)
#define FMAX            4000  // max freq (Hz)
#define MFCC_BANK_CNT   40    // number of mfcc filter banks
#define DO_DCT          1

#define LIFTER_COEFF    0
