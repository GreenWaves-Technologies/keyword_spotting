/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdlib.h>
#include <math.h>
#include "GenLUT.h"

typedef   signed short v2s __attribute__((vector_size (4)));

  /* Setup twiddles factors */

void SetupTwiddlesLUT(signed short *Twiddles, int Nfft, int Inverse)
{
  int i;
  v2s *P_Twid = (v2s *) Twiddles;

  /*
    Radix 4: 3/4 of the twiddles
    Radix 2: 1/2 of the twiddles 
  */

  if (Inverse) {
    float Theta = (2*M_PI)/Nfft;
    for (i=0; i<Nfft; i++) {
      float Phi = Theta*i;
      P_Twid[i] = (v2s) {(short int) (cos(Phi)*((1<<FFT_TWIDDLE_DYN)-1)),
                         (short int) (sin(Phi)*((1<<FFT_TWIDDLE_DYN)-1))};
    }
  } else {
    float Theta = (2*M_PI)/Nfft;
    for (i=0; i<Nfft; i++) {
      float Phi = Theta*i;
      P_Twid[i] = (v2s) {(short int) (cos(-Phi)*((1<<FFT_TWIDDLE_DYN)-1)),
                         (short int) (sin(-Phi)*((1<<FFT_TWIDDLE_DYN)-1))};
    }
  }
}

/* Setup a LUT for digitally reversed indexed, base is 4 */
void SetupR4SwapTable(short int *SwapTable, int Ni)
{
  int iL, iM, i, j;
  int Log4N  = gap_fl1(Ni)>>1;

  iL = Ni / 4; iM = 1;
  SwapTable[0] = 0;

  for (i = 0; i < Log4N; ++i) {
    for (j = 0; j < iM; ++j) {
      SwapTable[    iM + j] = SwapTable[j] +     iL;
      SwapTable[2 * iM + j] = SwapTable[j] + 2 * iL;
      SwapTable[3 * iM + j] = SwapTable[j] + 3 * iL;
    }
    iL >>= 2; iM <<= 2;
  }
}

void SetupR2SwapTable(short int *SwapTable, int Ni)
{
  int i, j, iL, iM;
  int Log2N  = gap_fl1(Ni);

  iL = Ni / 2;
  iM = 1;
  SwapTable[0] = 0;

  for (i = 0; i < Log2N; ++i) {
    for (j = 0; j < iM; ++j) SwapTable[j + iM] = SwapTable[j] + iL;
    iL >>= 1; iM <<= 1;
  }
}

void SetupTwidend(short int *twidend, int Ni)
{
	for (int i=0; i<Ni;i++) {
	  twidend[2*i]   = FP2FIXR( cosf(2*M_PI*i/(4*Ni)),FFT_TWIDDLE_DYN);
	  twidend[2*i+1] = FP2FIXR(-sinf(2*M_PI*i/(4*Ni)),FFT_TWIDDLE_DYN);
	}
}

void SetupLiftCoeff(short int *__restrict__ LiftCoeff, int FrameSize, int L) {
  for (int i=0; i<FrameSize;i++) {
    LiftCoeff[i] = (short int)( (1.0 + (L/2.0)*sinf(M_PI*i/L))*((1<< 11)));
  }
}

void SetupHammingWindow(short int * __restrict__ HammingLUT, int FrameSize)
{
	unsigned int i;

  	for (i=0; i<FrameSize; i++) {
		HammingLUT[i] = FP2FIX(0.54 - 0.46 * cos((2.0 * M_PI / (FrameSize - 1)) * (i)), 15);
	}
}

void SetupHanningWindow(short int * __restrict__ HanningLUT, int FrameSize)
{
  unsigned int i;

  for (i=0; i<FrameSize; i++) {
    HanningLUT[i] = FP2FIX(0.5 - 0.5 * cos((2.0 * M_PI / (FrameSize - 1)) * (i)), 15);
  }
}

void SetupDCTTable(short int *dct_coeff, int N_dct)
{
  for (int k=0; k<N_dct; k++){
    for (int i=0; i<N_dct; i++){
      dct_coeff[k*N_dct+i] = FP2FIXR(cosf(M_PI/N_dct * k * (i + 0.5)), 15);
    }
  }
}