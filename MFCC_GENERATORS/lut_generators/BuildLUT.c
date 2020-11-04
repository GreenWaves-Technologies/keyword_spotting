//#include "Gap8.h"
//#include "../FFT_Lib/FFT_Lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "GenLUT.h"

void BuildLUT(char *OutputDirName, int FrameSize, int NumFFT, int NumDCT, int NumCEP, int DoDCT, int FFTRadixType, int L, fn_window_t Window_fn)

{
	int i;
	FILE *fi;
	short int WindowLUT[2*FrameSize];
	short int TwiddlesLUT[NumFFT*2];
	short int SwapLUT[NumFFT];
	short int Twidend[NumDCT];
	short int DCTCoeff[NumDCT*NumDCT];
	short int LiftCoeff[NumCEP];

	char FileName[100];
	sprintf(FileName, "%s/LUT.def", OutputDirName);
	fi = fopen(FileName, "wb");

	// Windowing
	if (Window_fn==HAMMING_FN)	    SetupHammingWindow(WindowLUT, FrameSize);
	else if (Window_fn==HANNING_FN)	SetupHanningWindow(WindowLUT, FrameSize);
	else printf("Window function not known");

	fprintf(fi, " short int WindowLUT[%d] = {\n\t", FrameSize);
	for (i=0; i<FrameSize;i++) {
		fprintf(fi, "%d, ", WindowLUT[i]);
		if (((i+1)%12)==0) fprintf(fi, "\n\t");
	}
	fprintf(fi, "\n};\n");

	// Twiddles for first FFT
	SetupTwiddlesLUT(TwiddlesLUT, NumFFT, 0);
	fprintf(fi, " short int TwiddlesLUT[%d] = {\n\t", NumFFT);
	for (i=0; i<(NumFFT/2);i++) {
		fprintf(fi, "%d, %d, \n\t", TwiddlesLUT[2*i], TwiddlesLUT[2*i+1]);
	}
	fprintf(fi, "\n};\n");

	// SwapLUT for first FFT
	(FFTRadixType)?SetupR4SwapTable(SwapLUT, NumFFT):SetupR2SwapTable(SwapLUT, NumFFT);
	fprintf(fi, " short int SwapLUT[%d] = {\n\t", NumFFT);
	for (i=0; i<NumFFT;i++) {
		fprintf(fi, "%d, ", SwapLUT[i]);
		if (((i+1)%13)==0) fprintf(fi, "\n\t");
	}
	fprintf(fi, "\n};\n");

	if(DoDCT){
		// DCT twiddles (twice the size of the DCT)
		SetupTwiddlesLUT(TwiddlesLUT, NumDCT*2, 0);
		fprintf(fi, " short int Twiddles_dct[%d] = {\n\t", 2*NumDCT);
		for (i=0; i<NumDCT;i++) {
			fprintf(fi, "%d, %d, ", TwiddlesLUT[2*i], TwiddlesLUT[2*i+1]);
			if (((i+1)%6)==0) fprintf(fi, "\n\t");
		}
		fprintf(fi, "\n};\n");

		// DCT bit reverse table twice size of the DCT
		(FFTRadixType)?SetupR4SwapTable(SwapLUT, NumDCT*2):SetupR2SwapTable(SwapLUT, NumDCT*2);
		fprintf(fi, "  short int SwapLUT_dct[2*%d] = {\n\t", NumDCT);
		for (i=0; i<2*NumDCT;i++) {
			fprintf(fi, "%d, ", SwapLUT[i]);
			if (((i+1)%13)==0) fprintf(fi, "\n\t");
		}
		fprintf(fi, "\n};\n");


		// DCT final twiddle stage twice size of the DCT
		SetupTwidend(Twidend,NumDCT);
		fprintf(fi, " short int twidend_dct[2*%d] = {\n\t", NumDCT);
		for (i=0; i<NumDCT;i++) {
		  fprintf(fi, "%d, %d, ",Twidend[2*i], Twidend[2*i+1]);
		  if (((i+1)%6)==0) fprintf(fi, "\n\t");
		}
		fprintf(fi, "\n};\n");

		// DCT final twiddle stage twice size of the DCT
		SetupDCTTable(DCTCoeff, NumDCT);
		fprintf(fi, " short int DCT_Coeff[%d*%d] = {\n\t", NumDCT, NumDCT);
		for (i=0; i<NumDCT*NumDCT;i++) {
		  fprintf(fi, "%d, ", DCTCoeff[i]);
		  if (((i+1)%NumDCT)==0) fprintf(fi, "\n\t");
		}
		fprintf(fi, "\n};\n");

		// lifter coefficients
		printf("Building Lifter table L: %d\n", L);
		fprintf(fi, "short int lift_coeff[%d] = {\n\t", NumCEP);
		for (i=0; i<NumCEP;i++) {
		  if (L>0) fprintf(fi, "%d, ", (short int)( (1.0 + (L/2.0)*sinf(M_PI*i/L))*((1<<11))));
		  else fprintf(fi, "%d, ", (short int) (1<<11));
		  if (((i+1)%6)==0) fprintf(fi, "\n\t");
		}
		fprintf(fi, "\n};\n");
	}

	fclose(fi);
}
