/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "AutoTilerLibTypes.h"
#include "MFCC_Generator.h"

void LoadMFCCLibrary()

{

       LibKernel("Radix2FFT_DIF_Par", CALL_PARALLEL,
		 CArgs(4,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft")
		       ),
		 "FFT_Arg_T",NULL
		 );

       LibKernel("Radix2FFT_DIF_INT_Scal_Par", CALL_PARALLEL,
		 CArgs(4,
		       TCArg("int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft")
		       ),
		 "FFT_scal_Arg_T",NULL
		 );
       
       LibKernel("RadixMixedFFT_DIF_Par", CALL_PARALLEL,
		 CArgs(4,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft")
		       ),
		 "FFT_Arg_T",NULL
		 );
       
        LibKernel("RadixMixedFFT_DIF_Inv_Par", CALL_PARALLEL,
		 CArgs(4,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft")
		       ),
		 "FFT_Arg_T",NULL
		 );
	
	LibKernel("SwapSamples_Par", CALL_PARALLEL,
		 CArgs(3,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "SwapTable"),
		       TCArg("int"   , "Ni")
		       ),
		 "SwapSamples_Arg_T",NULL
		 );

	LibKernel("SwapSamples_scal", CALL_PARALLEL,
		 CArgs(4,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "SwapTable"),
		       TCArg("signed char * __restrict__"   , "shift_BF"),
		       TCArg("int"   , "Ni")
		       ),
		 "SwapSamples_scal_Arg_T",NULL
		 );

        LibKernel("MFCC_Step_args", CALL_PARALLEL,
		  CArgs(8,
			TCArg("short int * __restrict__"   , "Insignal"),
			TCArg("short int * __restrict__"   , "FEAT_LIST"),
			TCArg("short int * __restrict__"   , "out_fft"),
			TCArg("int16_t * __restrict__", "Twiddles_fft"),
			TCArg("int16_t * __restrict__", "SwapTable_fft"),
			TCArg("int16_t * __restrict__", "WinTable"),
			TCArg("fbank_type_t * __restrict__", "filterbank"),
			TCArg("int16_t * __restrict__", "MFCC_Coeffs")
			),
		  "MFCC_Args_T",NULL
	);

        LibKernel("MFCC_WindowedFrame", CALL_PARALLEL,
		  CArgs(5,
			TCArg("short int * __restrict__"   , "Frame"),
			TCArg("short int * __restrict__"   , "OutFrame"),
			TCArg("short int * __restrict__"   , "Window"),
			TCArg("int16_t", "FrameSize"),
			TCArg("int16_t", "FFT_Dim")
			),
		  "MFCC_WF_T",NULL
	);

        LibKernel("MFCC_WindowedFrame_int", CALL_PARALLEL,
		  CArgs(5,
			TCArg("short int * __restrict__"   , "Frame"),
			TCArg("int       * __restrict__"   , "OutFrame"),
			TCArg("short int * __restrict__"   , "Window"),
			TCArg("int16_t", "FrameSize"),
			TCArg("int16_t", "FFT_Dim")
			),
		  "MFCC_WFINT_T",NULL
		  );

        LibKernel("MFCC_PreEmphasis", CALL_PARALLEL,
		  CArgs(5,
			TCArg("short int * __restrict__"   , "Frame"),
			TCArg("short int * __restrict__"   , "Out"),
			TCArg("short int", "Prev"),
			TCArg("short int *", "Shift"),
			TCArg("short int", "FrameSize")
			),
		  "MFCC_PreEmphasis_T",NULL
		  );

        LibKernel("MFCC_Power", CALL_PARALLEL,
		  CArgs(4,
			TCArg("v2s * __restrict__"   , "FrameIn"),
			TCArg("int * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_T",NULL
		  );

        LibKernel("MFCC_PowerV2S", CALL_PARALLEL,
		  CArgs(4,
			TCArg("v2s * __restrict__"   , "FrameIn"),
			TCArg("int * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_T",NULL
		  );

        LibKernel("MFCC_ComputeLog", CALL_PARALLEL,
		  CArgs(5,
			TCArg("unsigned int * __restrict__"   , "FrameIn"),
			TCArg("unsigned int"   , "FrameSize"),
			TCArg("short int *"   , "Shift"),
			TCArg("short int"   , "offshift"),
			TCArg("signed char * __restrict__"   , "shift_BF")
			),
		  "MFCC_Log_T",NULL
		  );

        LibKernel("MFCC_ComputeMFCC", CALL_PARALLEL,
		  CArgs(5,
			TCArg("unsigned int * __restrict__"   , "FramePower"),
			TCArg("unsigned int * __restrict__"   , "MFCC"),
			TCArg("fbank_type_t *__restrict__", "MFCC_FilterBank"),
			TCArg("short int *__restrict__", "MFCC_Coeffs"),
			TCArg("short int","MFCC_bank_cnt")
			),
		  "MFCC_MF_T",NULL
		  );

        LibKernel("MFCC_ComputeMFCC_BFF", CALL_PARALLEL,
		  CArgs(7,
			TCArg("unsigned int * __restrict__"   , "FramePower"),
			TCArg("unsigned int * __restrict__"   , "MFCC"),
			TCArg("fbank_type_t *__restrict__", "MFCC_FilterBank"),
			TCArg("short int *__restrict__", "MFCC_Coeffs"),
			TCArg("signed char *__restrict__", "shift_pow"),
			TCArg("signed char *__restrict__", "shift_BF"),
			TCArg("short int","MFCC_bank_cnt")
			),
		  "MFCC_MF_New_T",NULL
		  );
	
        LibKernel("MFCC_ComputeDCT", CALL_PARALLEL,
		  CArgs(8,
			TCArg("void * __restrict__ ", "Data"),	
			TCArg("void * __restrict__ ", "Twiddles"),	
			TCArg("void * __restrict__ ", "Twidend"),	
			TCArg("short int * __restrict__", "SwapLuts"),	
			TCArg("short int * __restrict__", "Lift_coeff"),	
			TCArg("short int * __restrict__", "FeatList"),
			TCArg("short int", "n_dct"),
			TCArg("short int", "numcep")
			),
		  "DCT_Arg_T",NULL
		  );

        LibKernel("MFCC_ComputeDCT_II", CALL_PARALLEL,
		  CArgs(6,
			TCArg("void * __restrict__ ", "Data"),	
			TCArg("void * __restrict__ ", "DCTCoeff"),
			TCArg("short int * __restrict__", "Lift_coeff"),	
			TCArg("short int * __restrict__", "FeatList"),
			TCArg("short int", "n_dct"),
			TCArg("short int", "numcep")
			),
		  "DCT_II_Arg_T",NULL
		  );
		
        LibKernel("get_max", CALL_PARALLEL,
		  CArgs(4,
			TCArg("int * __restrict__"   , "Frame"),
			TCArg("int * __restrict__"   , "maxin"),
			TCArg("int", "FrameSize"),
			TCArg("int", "shift")
			),
		  "get_max_T",NULL
		  );

        LibKernel("Coeff_mult", CALL_PARALLEL,
		  CArgs(4,
			TCArg("unsigned int * __restrict__"   , "In1"),
			TCArg("short int * __restrict__"   , "In2"),
			TCArg("short int * __restrict__"   , "Out"),
			TCArg("int", "FrameSize")
			),
		  "COEFF_mult_args_T",NULL
		  );
        LibKernel("IMEL", CALL_PARALLEL,
		  CArgs(6,
			TCArg("short int * __restrict__"   , "In"),
			TCArg("short int * __restrict__"   , "Out"),
			TCArg("short int * __restrict__"   , "IMFCC_FilterBank"),
			TCArg("short int * __restrict__"   , "IMFCC_Coeffs"),
			TCArg("int", "nbin"),
			TCArg("int", "nbout")
			),
		  "IMEL_args_T",NULL
	);
        LibKernel("norm_clip_16", CALL_PARALLEL,
		  CArgs(3,
			TCArg("short int * __restrict__"   , "Out"),
			TCArg("unsigned short int", "Norm"),
			TCArg("int", "N")
			),
		  "Norm_Clip_args_T",NULL
	);
}

void GeneratorMFCC(char *Name, int FrameSize, int n_fft, int numcep, int mfcc_coeff_cnt, int do_dct, int do_lifter)

{

  int padfft;

  if (__builtin_popcount(n_fft) != 1) GenTilingError("MFCC: %s, Incorrect fft size: %d, it has to be a a power of 2", Name, n_fft);

  // define padding for in place computation: The input buffer xill be used to compute the FFT (v2s)
  padfft = (2*n_fft-FrameSize>=0)?(2*n_fft-FrameSize):0;

  UserKernel(Name,
	     KernelIterSpace(1, IterTiledSpace(KER_TILE)),
	     TILE_HOR,
	     CArgs(8,
		   TCArg("int16_t * __restrict__", "In"),
		   TCArg("int16_t * __restrict__", "Out"),
		   TCArg("int16_t * __restrict__", "Out_fft"),
		   TCArg("int16_t * __restrict__", "Twiddles_fft"),
		   TCArg("int16_t * __restrict__", "SwapTable_fft"),
		   TCArg("int16_t * __restrict__", "WinTable"),
		   TCArg("fbank_type_t * __restrict__", "filterbank"),
		   TCArg("int16_t * __restrict__", "MFCC_Coeffs")
		   ),
	     Calls(1,
		   Call("MFCC_Step_args", LOC_LOOP,
			Bindings(8,
				 K_Arg("KerIn", KER_ARG_TILE),
				 K_Arg("KerOut", KER_ARG_TILE),
				 K_Arg("KerOut", KER_ARG_TILE),
				 C_Arg("Twiddles_fft"),
				 C_Arg("SwapTable_fft"),
				 C_Arg("WinTable"),
				 C_Arg("filterbank"),
				 C_Arg("MFCC_Coeffs")
				 )
			)
		   ),
	     KerArgs(3,
		     KerArgPad("KerIn",  KerArgSpace(1,KER_TILE), OBJ_IN_DB,  1, FrameSize+1, padfft, 0, sizeof(short int),  0, 0, 0, "In"),
		     KerArg("KerOut", KerArgSpace(1,KER_TILE), OBJ_OUT_DB, 1, numcep, sizeof(short int),  0, 0, 0, "Out"),
		     KerArg("KerOut", KerArgSpace(1,KER_TILE), OBJ_OUT_DB, 1, n_fft, 2*sizeof(short int),  0, 0, 0, "Out_fft")
		     )

	     );

}

void MFCC_parallel(char *Name, int FrameSize, int n_fft, int numcep, int mfcc_coeff_cnt, int do_dct, int do_lifter, int n_dct)
{

  int nbcalls = do_dct?9:8;
  int nb_cargs = do_dct?18:13;
  //nb_cargs = do_lifter?nb_cargs+1:nb_cargs;
  //nbcalls = do_lifter?nbcalls+1:nbcalls;
  CKernel_Arg_T **KCArgs = AllocateCArgs(nb_cargs);

  // check that inout1 buffer (of size 2*n_fft*sizeof(short int)) is big enough to contain the DCT (of size 2*2*n_dct*sizeof(short int) )
  if (do_dct) {
    if ((4 * n_dct) > (2 * n_fft))  printf("MFCC_parallel generator: The size of dct (%d) is > the size of fft buffer (%d)\n", 4*n_dct,2*n_fft);
  }
  
  int Ca=0;

  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "In");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "Out");
  KCArgs[Ca++] =  TCArg("int16_t", "Prev");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "Twiddles_fft");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "SwapTable_fft");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "WinTable");
  KCArgs[Ca++] =  TCArg("fbank_type_t * __restrict__", "MFCC_FilterBank");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "MFCC_Coeffs");
  KCArgs[Ca++] =  TCArg("int16_t", "FrameSize");
  KCArgs[Ca++] =  TCArg("int16_t", "nfft");
  KCArgs[Ca++] =  TCArg("int16_t", "offshift");
  KCArgs[Ca++] =  TCArg("int16_t", "numcep");
  KCArgs[Ca++] =  TCArg("int16_t", "Norm");
  if (do_dct) {
    KCArgs[Ca++] =  TCArg("int16_t", "n_dct");
    KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "DCT_Coeff");
    // KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "Twiddles_dct");
    // KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "SwapLUT_dct");
    // KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "Twidend_dct");
    KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "lift_coeff");
  }


	UserKernel(Name,
		   KernelIterSpace(1, IterTiledSpace(KER_TILE)),
		   TILE_HOR,
		   KCArgs,
		   Calls(nbcalls,
			 Call("MFCC_PreEmphasis",LOC_LOOP,
			      Bindings(5,
				       K_Arg("In"   , KER_ARG_TILE),
				       K_Arg("inout1"   , KER_ARG_TILE),
				       C_Arg("Prev"),
				       K_Arg("shiftbuf"   , KER_ARG_TILE),
				       C_Arg("FrameSize")
				       )
			      ),
#ifdef HIGH_PREC_FFT
			 Call("MFCC_WindowedFrame_int",LOC_LOOP,
			      Bindings(5,
				       K_Arg("inout1"  , KER_ARG_TILE),
				       K_Arg("Out_fft" , KER_ARG_TILE),
				       K_Arg("WinTable" , KER_ARG_TILE ),
				       C_Arg("FrameSize"),
				       C_Arg("nfft")
				       )
			      ),
			 Call("Radix2FFT_DIF_INT_Scal_Par",LOC_LOOP,
			      Bindings(4,
				       K_Arg("Out_fft"     , KER_ARG_TILE),
				       K_Arg("Twiddles_fft", KER_ARG_TILE),
				       K_Arg("shift_fft"    , KER_ARG_TILE),
				       C_Arg("nfft")
				       )
			      ),
			 Call("SwapSamples_scal",LOC_LOOP,
			      Bindings(4,
				       K_Arg("Out_fft"     , KER_ARG_TILE),
				       C_Arg("SwapTable_fft"),
				       K_Arg("shift_fft"   , KER_ARG_TILE),
				       C_Arg("nfft")
				       )
			      ),
			 Call("MFCC_Power",LOC_LOOP,
			      Bindings(4,
				       K_Arg("Out_fft"   , KER_ARG_TILE),
				       K_Arg("inout1"   , KER_ARG_TILE),
				       K_Arg("shift_fft"   , KER_ARG_TILE),
				       C_Arg("nfft" )
				       )
			      ),
			Call("MFCC_ComputeMFCC_BFF", LOC_LOOP,
			      Bindings(7,
				       K_Arg("inout1"   	  , KER_ARG_TILE),
				       K_Arg("inout2"   	  , KER_ARG_TILE),
				       K_Arg("MFCC_FilterBank", KER_ARG_TILE),
				       K_Arg("MFCC_Coeffs"    , KER_ARG_TILE),
				       K_Arg("shift_fft"      , KER_ARG_TILE),
					   K_Arg("shift_bf"       , KER_ARG_TILE),
				       C_Arg("numcep")
				       )
			      ),
#else
			 Call("MFCC_WindowedFrame",LOC_LOOP,
			      Bindings(5,
				       K_Arg("inout1"  , KER_ARG_TILE),
				       K_Arg("Out_fft" , KER_ARG_TILE),
				       K_Arg("WinTable" , KER_ARG_TILE ),
				       C_Arg("FrameSize"),
				       C_Arg("nfft")
				       )
			      ),
			 Call("Radix2FFT_DIF_Par",LOC_LOOP,
			      Bindings(4,
				       K_Arg("Out_fft"   , KER_ARG_TILE),
				       K_Arg("Twiddles_fft" , KER_ARG_TILE ),
				       K_Arg("shift_fft"   , KER_ARG_TILE),
				       C_Arg("nfft")
				       )
			      ),
			 Call("SwapSamples_Par",LOC_LOOP,
			      Bindings(3,
				       K_Arg("Out_fft"   , KER_ARG_TILE),
				       K_Arg("SwapTable_fft"  , KER_ARG_TILE),
				       C_Arg("nfft" )
				       )
			      ),
			 Call("MFCC_PowerV2S",LOC_LOOP,
			      Bindings(4,
				       K_Arg("Out_fft"   , KER_ARG_TILE),
				       K_Arg("inout1"   , KER_ARG_TILE),
				       K_Arg("shift_fft"   , KER_ARG_TILE),
				       C_Arg("nfft" )
				       )
			      ),
			Call("MFCC_ComputeMFCC", LOC_LOOP,
			      Bindings(5,
				       K_Arg("inout1"         , KER_ARG_TILE),
				       K_Arg("inout2"         , KER_ARG_TILE),
				       K_Arg("MFCC_FilterBank", KER_ARG_TILE),
				       K_Arg("MFCC_Coeffs"    , KER_ARG_TILE),
				       C_Arg("numcep")
				       )
			      ),
#endif
			 Call("MFCC_ComputeLog",LOC_LOOP,
			      Bindings(5,
			 	       K_Arg("inout2"   , KER_ARG_TILE),
				       C_Arg("numcep"),
				       K_Arg("shiftbuf", KER_ARG_TILE),
				       C_Arg("offshift"),
			 	       K_Arg("shift_bf"   , KER_ARG_TILE)
				       )
			      ),
			 Call("norm_clip_16",LOC_LOOP,
			      Bindings(3,
				       K_Arg("inout2", KER_ARG_TILE),
				       C_Arg("Norm"),
				       C_Arg("numcep")
				       )
			      ),
			 do_dct?
			 Call("MFCC_ComputeDCT_II",LOC_LOOP,
			      Bindings(6,
			 	       K_Arg("inout2"   , KER_ARG_TILE),
			 	       K_Arg("DCT_Coeff"   , KER_ARG_TILE),
			 	       K_Arg("lift_coeff"   , KER_ARG_TILE),
			 	       K_Arg("inout1"   , KER_ARG_TILE),
				       C_Arg("n_dct"),
				       C_Arg("numcep")
				       )
			      ):AT_NO_CALL
#if 0
			 Call("MFCC_ComputeDCT",LOC_LOOP,
			      Bindings(8,
			 	       K_Arg("inout1"   , KER_ARG_TILE),
			 	       K_Arg("Twiddles_dct"   , KER_ARG_TILE),
			 	       K_Arg("Twidend_dct"   , KER_ARG_TILE),
			 	       K_Arg("SwapLUT_dct"   , KER_ARG_TILE),
			 	       K_Arg("lift_coeff"   , KER_ARG_TILE),
			 	       K_Arg("inout2"   , KER_ARG_TILE),
				       C_Arg("n_dct"),
				       C_Arg("numcep")
				       )
			      ):AT_NO_CALL
#endif

			 ),
		   KerArgs(do_dct?14:12,
			   KerArg("In",              KerArgSpace(1,KER_TILE), O_BUFF|O_IN,  1, FrameSize,      sizeof(short int),    0, 0, 0, "In"),
			   KerArg("Out_fft",         KerArgSpace(1,KER_TILE), O_BUFF,       1, 2*n_fft,        sizeof(int),          0, 0, 0, ""),
			   KerArg("inout2",          KerArgSpace(1,KER_TILE), O_BUFF,       1, n_dct,          sizeof(int),          0, 0, 0, ""),
			   KerArg("inout1",          KerArgSpace(1,KER_TILE), O_BUFF|O_OUT, 1, 2*n_fft,        sizeof(short int),    0, 0, 0, "Out"),
			   KerArg("shift_fft",       KerArgSpace(1,KER_TILE), O_BUFF,       1, 2*n_fft,        sizeof(signed char),  0, 0, 0, ""),
			   KerArg("shift_bf",       KerArgSpace(1,KER_TILE), O_BUFF,       1, n_fft,          sizeof(signed char),  0, 0, 0, ""),
			   KerArg("shiftbuf",        KerArgSpace(1,KER_TILE), O_BUFF,       1, 1,              sizeof(short int),    0, 0, 0, ""),
			   KerArg("WinTable",        KerArgSpace(1,KER_TILE), O_IN|O_BUFF,  1, n_fft,          sizeof(short int),    0, 0, 0, "WinTable"),
			   KerArg("Twiddles_fft",    KerArgSpace(1,KER_TILE), O_IN|O_BUFF,  1, 2*(3*n_fft/4),  sizeof(short int),    0, 0, 0, "Twiddles_fft"),
			   KerArg("SwapTable_fft",   KerArgSpace(1,KER_TILE), O_IN|O_BUFF,  1, n_fft,          sizeof(short int),    0, 0, 0, "SwapTable_fft"),
			   KerArg("MFCC_FilterBank", KerArgSpace(1,KER_TILE), O_IN|O_BUFF,  1, numcep,         sizeof(fbank_type_t), 0, 0, 0, "MFCC_FilterBank"),
			   KerArg("MFCC_Coeffs",     KerArgSpace(1,KER_TILE), O_IN|O_BUFF,  1, mfcc_coeff_cnt, sizeof(short int),    0, 0, 0, "MFCC_Coeffs"),

			   KerArg("DCT_Coeff",       KerArgSpace(1,KER_TILE), O_IN|O_BUFF,  1, n_dct*n_dct,    sizeof(short int),    0, 0, 0, "DCT_Coeff"),
#if 0
			   KerArg("Twiddles_dct",    KerArgSpace(1,KER_TILE), O_IN|O_BUFF,  1, 2*n_dct,        sizeof(short int),    0, 0, 0, "Twiddles_dct"),
			   KerArg("Twidend_dct",     KerArgSpace(1,KER_TILE), O_IN|O_BUFF,  1, 2*n_dct,        sizeof(short int),    0, 0, 0, "Twidend_dct"),
			   KerArg("SwapLUT_dct",     KerArgSpace(1,KER_TILE), O_IN|O_BUFF,  1, 2*n_dct,        sizeof(short int),    0, 0, 0, "SwapLUT_dct"),
#endif
			   KerArg("lift_coeff",      KerArgSpace(1,KER_TILE), O_IN|O_BUFF,  1, numcep,         sizeof(short int),    0, 0, 0, "lift_coeff")
			   
			   
			   )
		   );

}


void IMFCC_parallel(char *Name, int nbin, int n_fft,  int imfcc_coeff_cnt) {

  int nb_cargs = 10;
  int nbcalls = 4;
  CKernel_Arg_T **KCArgs = AllocateCArgs(nb_cargs);
  int Ca=0;

  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "In");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "Out");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "In_fft");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "Twiddles_inv_fft");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "SwapTable_fft");
  KCArgs[Ca++] =  TCArg("fbank_type_t * __restrict__", "IMFCC_FilterBank");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "IMFCC_Coeffs");
  KCArgs[Ca++] =  TCArg("int16_t", "nbin");
  KCArgs[Ca++] =  TCArg("int16_t", "nfft");
  KCArgs[Ca++] =  TCArg("int16_t", "numcep");


	UserKernel(Name,
		   KernelIterSpace(1, IterTiledSpace(KER_TILE)),
		   TILE_HOR,
		   KCArgs,
		   Calls(nbcalls,
			 Call("IMEL",LOC_LOOP,
			      Bindings(6,
				       K_Arg("In"   , KER_ARG_TILE),
				       K_Arg("OMEL"   , KER_ARG_TILE),
				       C_Arg("IMFCC_FilterBank" ),
				       C_Arg("IMFCC_Coeffs" ),
				       C_Arg("nbin"),
				       C_Arg("nfft")
				       )
			      ),
			 Call("Coeff_mult",LOC_LOOP,
			      Bindings(4,
				       K_Arg("OMEL"   , KER_ARG_TILE),
				       K_Arg("In_fft"   , KER_ARG_TILE),
				       K_Arg("Out"   , KER_ARG_TILE),
				       C_Arg("nfft" )
				       )
			      ),
			 Call("RadixMixedFFT_DIF_Par",LOC_LOOP,
			      Bindings(3,
				       K_Arg("Out"   , KER_ARG_TILE),
				       C_Arg("Twiddles_inv_fft" ),
				       C_Arg("nfft")
				       )
			      ),
			 Call("SwapSamples_Par",LOC_LOOP,
			      Bindings(3,
				       K_Arg("Out"   , KER_ARG_TILE),
				       C_Arg("SwapTable_fft" ),
				       C_Arg("nfft" )
				       )
			      )
			 ),
		   KerArgs(8,

			   KerArg("In", KerArgSpace(1,KER_TILE), O_BUFF|O_IN, 1, nbin, sizeof(short int), 0, 0, 0, "In"),
			   KerArg("In_fft", KerArgSpace(1,KER_TILE), O_BUFF|O_IN, 1, 2*n_fft, sizeof(short int), 0, 0, 0, "In_fft"),
			   KerArg("Out", KerArgSpace(1,KER_TILE), O_BUFF|O_OUT, 1, 2*n_fft, sizeof(short int), 0, 0, 0, "Out"),
			   KerArg("OMEL", KerArgSpace(1,KER_TILE), O_BUFF, 1, n_fft, sizeof(short int), 0, 0, 0, ""),
			   KerArg("Twiddles_inv_fft", KerArgSpace(1,KER_TILE), O_IN|O_DB|O_CONST, 1, 2*n_fft, sizeof(short int), 0, 0, 0, "Twiddles_inv_fft"),
			   KerArg("SwapTable_fft", KerArgSpace(1,KER_TILE), O_IN|O_DB|O_CONST, 1, n_fft, sizeof(short int), 0, 0, 0, "SwapTable_fft"),
			   KerArg("IMFCC_FilterBank", KerArgSpace(1,KER_TILE), O_IN|O_DB|O_CONST, 1, nbin, sizeof(fbank_type_t), 0, 0, 0, "IMFCC_FilterBank"),
			   KerArg("IMFCC_Coeffs", KerArgSpace(1,KER_TILE), O_IN|O_DB|O_CONST, 1, imfcc_coeff_cnt, sizeof(short int), 0, 0, 0, "IMFCC_Coeffs")
			   )
		   );
}

