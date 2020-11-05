/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s 

#ifndef __EMUL__
	/* PMSIS includes. */
	#include "pmsis.h"
#else
	#define pmsis_exit exit
	#define pi_l2_malloc malloc
	#define pi_l2_free(ptr,size) free(ptr)
#endif  /* __EMUL__ */

/* Autotiler includes. */
#include "Gap.h"
#ifdef SMALL
	#include "KWS_ds_cnn_s_quantKernels.h"
#endif
#ifdef MEDIUM
    #include "KWS_ds_cnn_m_quantKernels.h"
#endif
#ifdef LARGE
	#include "KWS_ds_cnn_l_quantKernels.h"
#endif
#include "wavIO.h"
#include "MFCC_params.h"
#include "MFCCKernels.h"
#include "LUT.def"
#include "MFCC_FB.def"

#define  WAV_BUFFER_SIZE        17000 // Something more than 1sec@16kHz
//DCT_NORMALIZATION        -> np.sqrt(1/(N_DCT))*0.5
//NNTOOL_INPUT_SCALE_FLOAT -> 1.9372712
// SCALE = NNTOOL_INPUT_SCALE_FLOAT*DCT_NORMALIZATION
#ifdef LARGE
    //with N_DCT=40
    #define  INPUT_SCALE        157
    #define  INPUT_SCALEN       10
#else
    //with N_DCT=10
    #define  INPUT_SCALE        157
    #define  INPUT_SCALEN       9
#endif


typedef signed char KWS_IMAGE_IN_T;
L2_MEM short int *ResOut;
char *WavName = NULL;
L2_MEM KWS_IMAGE_IN_T *ImageIn;
int off_shift = 0;
AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;

int num_samples;
short int *mfcc_features;
short int *inSig;

static void RunMFCC(){
    L1_Memory = (AT_L1_POINTER) AT_L1_ALLOC(0, _L1_Memory_SIZE);
    if (L1_Memory==NULL){
        printf("Error allocating L1\n");
        pmsis_exit(-1);
    }
    printf("Runnning MFCC\n");
    #ifdef PERF
        gap_cl_starttimer();
        gap_cl_resethwtimer();
        int start, elapsed, total_cyc;
        total_cyc = 0;
    #endif
    for (int i=0, j=0; i<N_FRAME*FRAME_STEP; i+=FRAME_STEP,j+=N_DCT){
        #ifdef PERF
            start = gap_cl_readhwtimer();
        #endif
        MFCC00(inSig+i, mfcc_features+j, (i)?inSig[i-1]:0, TwiddlesLUT, SwapLUT, WindowLUT, MFCC_FilterBank,
                    MFCC_Coeffs, FRAME_SIZE, N_FFT, off_shift, NUMCEP, 5, N_DCT, DCT_Coeff, lift_coeff);
        #ifdef PERF
            elapsed = gap_cl_readhwtimer() - start;
            total_cyc += elapsed;
        #endif
    }
    #ifdef PERF
      printf("MFCC Total Cycles: %d\n\n\n", total_cyc);
    #endif

    AT_L1_FREE(0, L1_Memory, _L1_Memory_SIZE);
}

static void Runkws()
{
  printf("Running on cluster\n");
#ifdef PERF
  gap_cl_starttimer();
  gap_cl_resethwtimer();
#endif
  __PREFIX(CNN)(ImageIn, ResOut);
  printf("Runner completed\n");

  //Checki Results
  int rec_digit = 0;
  int highest = ResOut[0];
  printf("Results: \n");
  for(int i = 0; i < 12; i++) {
    if(ResOut[i] > highest) {
      highest = ResOut[i];
      rec_digit = i;
    }
    printf("class %d: %d\n", i, ResOut[i]);
  }
  printf("\n");

  printf("Recognized:\t%d\n", rec_digit);
}


void kws_ds_cnn(void)
{
    printf("Entering main controller\n");

    #ifndef __EMUL__
        /* Configure And open cluster. */
        struct pi_device cluster_dev;
        struct pi_cluster_conf cl_conf;
        cl_conf.id = 0;
        pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
        if (pi_cluster_open(&cluster_dev))
        {
            printf("Cluster open failed !\n");
            pmsis_exit(-4);
        }
    #endif  /* __EMUL__ */
    
    ResOut        = (KWS_IMAGE_IN_T *) pi_l2_malloc(12                               * sizeof(KWS_IMAGE_IN_T));
    ImageIn       = (char *)           pi_l2_malloc(AT_INPUT_WIDTH * AT_INPUT_HEIGHT * sizeof(KWS_IMAGE_IN_T));
    mfcc_features = (short int *)      pi_l2_malloc(N_FRAME * N_DCT                  * sizeof(short int));
    inSig         = (short int *)      pi_l2_malloc(WAV_BUFFER_SIZE                  * sizeof(short int));
    if (mfcc_features==NULL || ImageIn==NULL || inSig==NULL || ResOut==NULL){
        printf("Error allocating output\n");
        pmsis_exit(1);
    }

    header_struct header_info;
    if (ReadWavFromFile(WavName, inSig, WAV_BUFFER_SIZE*sizeof(short int), &header_info)){
        printf("Error reading wav file\n");
        pmsis_exit(1);
    }
    num_samples = header_info.DataSize * 8 / (header_info.NumChannels * header_info.BitsPerSample);

    #ifndef __EMUL__
        struct pi_cluster_task task_mfcc = {0};
        task_mfcc.entry = RunMFCC;
        task_mfcc.arg = NULL;
        task_mfcc.stack_size = (unsigned int) STACK_SIZE;
        task_mfcc.slave_stack_size = SLAVE_STACK_SIZE;
        pi_cluster_send_task_to_cl(&cluster_dev, &task_mfcc);
        pi_l2_free(inSig, WAV_BUFFER_SIZE * sizeof(short int));
    #else
        RunMFCC();
    #endif

    for (int i=0; i<N_FRAME; i++) {
        for (int j=0; j<N_DCT; j++) {
            ImageIn[i*N_DCT+j] = (char) AT_SCALE(mfcc_features[i*N_DCT+j], INPUT_SCALE, INPUT_SCALEN);
        }
    }

    #ifdef PRINT_AT_INPUT
    printf("input_prescale_gap = np.array([\n");
    for (int i=0; i<N_FRAME; i++) {
        printf("[");
        for (int j=0; j<N_DCT; j++) {
            printf("%d, ", mfcc_features[i*N_DCT+j]);
        }
        printf("],\n");
    }
    printf("])\n");
    printf("input_gap = np.array([\n");
    for (int i=0; i<N_FRAME; i++) {
        printf("[");
        for (int j=0; j<N_DCT; j++) {
            printf("%d, ", ImageIn[i*N_DCT+j]);
        }
        printf("],\n");
    }
    printf("])\n");
    #endif
    #ifndef __EMUL__
        pi_l2_free(mfcc_features, N_FRAME * N_DCT * sizeof(short int));
    #endif


    printf("\n\nConstructor\n");
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    int err_construct = __PREFIX(CNN_Construct)();
    if (err_construct)
    {
        printf("Graph constructor exited with error: %d\n", err_construct);
        pmsis_exit(-5);
    }

    printf("Call cluster\n");
	#ifndef __EMUL__
		struct pi_cluster_task *task_net = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
		if(task_net==NULL) {
		  printf("pi_cluster_task alloc Error!\n");
		  pmsis_exit(-1);
		}
		printf("Stack size is %d and %d\n",STACK_SIZE,SLAVE_STACK_SIZE );
		memset(task_net, 0, sizeof(struct pi_cluster_task));
		task_net->entry = &Runkws;
		task_net->stack_size = STACK_SIZE;
		task_net->slave_stack_size = SLAVE_STACK_SIZE;
		task_net->arg = NULL;
		pi_cluster_send_task_to_cl(&cluster_dev, task_net);
	#else
	    Runkws();
	#endif
    #ifdef PERF
    {
        unsigned int TotalCycles = 0, TotalOper = 0;
        printf("\n");
        for (int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
            printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", AT_GraphNodeNames[i],
                   AT_GraphPerf[i], AT_GraphOperInfosNames[i], ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
            TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
        }
        printf("\n");
        printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
        printf("\n");
    }
    #endif  /* PERF */

    __PREFIX(CNN_Destruct)();

    #ifndef __EMUL__    
        pi_l2_free(ImageIn, AT_INPUT_WIDTH * AT_INPUT_HEIGHT * sizeof(KWS_IMAGE_IN_T));
        pi_l2_free(ResOut,  12*sizeof(KWS_IMAGE_IN_T));
        // Close the cluster
        pi_cluster_close(&cluster_dev);
    #endif
    printf("Ended\n");
    pmsis_exit(0);
}


#ifndef __EMUL__
int main()
{
	printf("\n\n\t *** NNTOOL KWS Example ***\n\n");

    #define __XSTR(__s) __STR(__s)
    #define __STR(__s) #__s
    WavName = __XSTR(AT_WAV);
    return pmsis_kickoff((void *) kws_ds_cnn);
}
#else
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: mnist [image_file]\n");
        exit(-1);
    }
    WavName = argv[1];
    printf("\n\n\t *** IMAGENET EMUL ***\n\n");
    kws_ds_cnn();
    return 0;
}
#endif