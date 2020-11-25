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
    #include <bsp/bsp.h>
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

#define  WAV_BUFFER_SIZE    16000 // Something more than 1sec@16kHz
#define  NUM_CLASSES        12

//DCT_NORMALIZATION        -> np.sqrt(1/(N_DCT))*0.5
//NNTOOL_INPUT_SCALE_FLOAT -> 1.9372712
// SCALE = NNTOOL_INPUT_SCALE_FLOAT*DCT_NORMALIZATION
#define  INPUT_SCALE        236
#define  INPUT_SCALEN       16

#define NB_ELEM 256
#define BUFF_SIZE (NB_ELEM*2)
#define ITER    64

static char *LABELS[NUM_CLASSES] = {"silence", "unknown", "yes", "no", "up", "down", "left", "right", "on", "off", "stop", "go"};
L2_MEM short int *ResOut;
char *WavName = NULL;
char *ImageIn;
int off_shift = 0;
AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;

int num_samples;
short int *mfcc_features;
short int *inSig;
int count, idx, end1, end2;
int rec_digit;


#ifdef FROM_SENSOR
    static uint16_t buff[2][NB_ELEM];
    static struct pi_device i2s;
    static int end = 0;
    static pi_task_t task;
    static short *chunk;
    #define LENGTH_AV 16
    #define SHL 5
    static int av[4][LENGTH_AV] ;
    static int idx_av=0;
    static int av_00=0,av_01=0,av_02=0,av_03=0;

    static void copy_data(uint8_t *dst, uint8_t *src, uint size)
    {
      memcpy(dst, src, size);
    }
    // dump one mono channel chunk (NB_ELEM/2 16bits samples) from the i2s0 interface in dump_buff:
    static void my_copy_data(uint16_t *dst, uint16_t *src, uint size)
    {
      int av_0=0,av_1=0,av_2=0,av_3=0;
      unsigned i, j=0;

      // copy 1 mono channel chunk into the dump buffer
      for ( i=0;i<size; i++){
        dst[i] = src[i]<<SHL;
        av_0 += dst[i];
      }

      // assume 256 samples in the buffer
      av_0 >>= 8;
      
      av_00 -= av[0][idx_av];
      av_00 += av_0;
      av[0][idx_av] = av_0;
      idx_av++;
      if (idx_av==LENGTH_AV) idx_av=0;

      // offset correction (length of buff of avg values is 16)
      for(i = 0; i < size; i++)
        {
          dst[i] -= (av_00>>4) ;
        }

    }

    // This callback is called when a transfer is finished
    // Just reenqueue another buffer unless we are done
    static void end_of_capture(void *arg)
    {
        unsigned int size;

        pi_i2s_read_status(&task, (void **)&chunk, &size);
        my_copy_data((uint16_t *)(inSig + idx * NB_ELEM), (uint16_t *) chunk, NB_ELEM);
        idx++;

        if (idx < (ITER+ITER/2)){
          pi_i2s_read_async(&i2s, pi_task_callback(&task, end_of_capture, NULL)); 
          if (idx == ITER) end1 = 1;
        }
        else {
          end2 = 1;
          idx = 0;
        }

    }
#endif

static void RunMFCC(){
    L1_Memory = KWS_ds_cnn_m_quant_L1_Memory;
    PRINTF("Runnning MFCC\n");
    #ifdef PERF
        gap_cl_starttimer();
        gap_cl_resethwtimer();
        int start, elapsed, total_cyc;
        total_cyc = 0;
        start = gap_cl_readhwtimer();
    #endif
    MFCC(inSig+(count-1)%2*WAV_BUFFER_SIZE/2, mfcc_features, 0, TwiddlesLUT, SwapLUT, WindowLUT, MFCC_FilterBank, MFCC_Coeffs, 5, N_DCT, DCT_Coeff);
    #ifdef PERF
        elapsed = gap_cl_readhwtimer() - start;
        total_cyc += elapsed;
        printf("MFCC Total Cycles: %d\n\n\n", total_cyc);
    #endif
}

static void Runkws()
{
  PRINTF("Running on cluster\n");
#ifdef PERF
  gap_cl_starttimer();
  gap_cl_resethwtimer();
#endif
  __PREFIX(CNN)(ImageIn, ResOut);

  //Checki Results
  rec_digit = 0;
  int highest = ResOut[0];
  PRINTF("Results: \n");
  for(int i = 0; i < NUM_CLASSES; i++) {
    if(ResOut[i] > highest) {
      highest = ResOut[i];
      rec_digit = i;
    }
    PRINTF("class %d: %d\n", i, ResOut[i]);
  }
  if (highest < 7000) rec_digit = 1;

  printf("Recognized:\t%s\n", LABELS[rec_digit]);
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
    
    ResOut        = (short int *) pi_l2_malloc(NUM_CLASSES                      * sizeof(short int));
    ImageIn       = (char *)      pi_l2_malloc(AT_INPUT_WIDTH * AT_INPUT_HEIGHT * sizeof(char));
    mfcc_features = (short int *) pi_l2_malloc(N_FRAME * N_DCT                  * sizeof(short int));
    inSig         = (short int *) pi_l2_malloc(NB_ELEM * (ITER + ITER/2)        * sizeof(short int));
    if (mfcc_features==NULL || ImageIn==NULL || inSig==NULL || ResOut==NULL){
        printf("Error allocating output\n");
        pmsis_exit(1);
    }


    printf("\n\nConstructor\n");
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    int err_construct = __PREFIX(CNN_Construct)();
    if (err_construct)
    {
        printf("Graph constructor exited with error: %d\n", err_construct);
        pmsis_exit(-5);
    }

    #ifdef FROM_SENSOR
        // Get default I2S interface config
        struct pi_i2s_conf i2s_conf;
        pi_i2s_conf_init(&i2s_conf);

        // Configure first interface for PDM 44100KHz DDR
        // Also gives the 2 buffers for double-buffering the sampling
        i2s_conf.pingpong_buffers[0] = buff[0];
        i2s_conf.pingpong_buffers[1] = buff[1];
        i2s_conf.block_size = NB_ELEM*sizeof(short);
        i2s_conf.frame_clk_freq = 16000;
        i2s_conf.itf = 0;
        i2s_conf.channels = 1;
        i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_PDM;
        i2s_conf.word_size = 16;

        pi_open_from_conf(&i2s, &i2s_conf);

        // Open the driver
        if (pi_i2s_open(&i2s))
          pmsis_exit(1);  

        // Start sampling, the driver will use the double-buffers we provided to store
        // the incoming samples
        if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL))
          pmsis_exit(1);
        pi_time_wait_us(1000);
    #endif

count=0;
idx = 0;
end1 = end2 = 0;
while(1)
{
    #ifndef FROM_SENSOR
        header_struct header_info;
        if (ReadWavFromFile(WavName, inSig, WAV_BUFFER_SIZE*sizeof(short int), &header_info)){
            printf("Error reading wav file\n");
            pmsis_exit(1);
        }
        num_samples = header_info.DataSize * 8 / (header_info.NumChannels * header_info.BitsPerSample);
    #else
        unsigned int size;

        // Once it returns, chunk will point to the next available buffer
        // containing samples.
        pi_i2s_read_async(&i2s, pi_task_callback(&task, end_of_capture, NULL));
        // Wait until acquisition is finished
        while(end1==0 && end2==0)
          {
            pi_yield();
          }
        count++;
        if (end1) end1 = 0;
        if (end2) end2 = 0;

        #ifdef WRITE_WAV
            char FileName[100];
            sprintf(FileName, "../../../from_gap_%d_%s.wav", count, LABELS[rec_digit]);
            WriteWavToFile(FileName, i2s_conf.word_size, i2s_conf.frame_clk_freq,
                 i2s_conf.channels, (void *)inSig+(count-1)%2*WAV_BUFFER_SIZE/2, WAV_BUFFER_SIZE * sizeof(short int));
        #endif
    #endif

    #ifndef __EMUL__
        struct pi_cluster_task task_mfcc = {0};
        task_mfcc.entry = RunMFCC;
        task_mfcc.arg = NULL;
        task_mfcc.stack_size = (unsigned int) STACK_SIZE;
        task_mfcc.slave_stack_size = SLAVE_STACK_SIZE;
        pi_cluster_send_task_to_cl(&cluster_dev, &task_mfcc);
        //pi_l2_free(inSig, WAV_BUFFER_SIZE * sizeof(short int));
    #else
        RunMFCC();
    #endif

    for (int i=0; i<N_FRAME; i++) {
        // Take only the first N_CEP that you need (in this case 10)
        for (int j=0; j<10; j++) {
            ImageIn[i*AT_INPUT_WIDTH+j] = (char) gap_roundnorm(mfcc_features[i*N_DCT+j]*INPUT_SCALE, INPUT_SCALEN);
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
                printf("%d, ", ImageIn[i*AT_INPUT_WIDTH+j]);
            }
            printf("],\n");
        }
        printf("])\n");
    #endif
    #ifndef __EMUL__
        //pi_l2_free(mfcc_features, N_FRAME * N_DCT * sizeof(short int));
    #endif

    PRINTF("Call cluster\n");
	#ifndef __EMUL__
		struct pi_cluster_task *task_net = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
		if(task_net==NULL) {
		  printf("pi_cluster_task alloc Error!\n");
		  pmsis_exit(-1);
		}
		//PRINTF("Stack size is %d and %d\n",STACK_SIZE,SLAVE_STACK_SIZE );
		memset(task_net, 0, sizeof(struct pi_cluster_task));
		task_net->entry = &Runkws;
		task_net->stack_size = STACK_SIZE;
		task_net->slave_stack_size = SLAVE_STACK_SIZE;
		task_net->arg = NULL;
		pi_cluster_send_task_to_cl(&cluster_dev, task_net);
	#else
	    Runkws();
        break;
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

}
    #ifdef FROM_SENSOR
        // Now stop sampling
        pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);
        // Close the driver
        pi_i2s_close(&i2s);
    #endif
    __PREFIX(CNN_Destruct)();

    #ifndef __EMUL__    
        pi_l2_free(ImageIn, AT_INPUT_WIDTH * AT_INPUT_HEIGHT * sizeof(char));
        pi_l2_free(ResOut,  NUM_CLASSES*sizeof(short int));
        // Close the cluster
        pi_cluster_close(&cluster_dev);
    #endif
    PRINTF("Ended\n");
    pmsis_exit(0);
}


#ifndef __EMUL__
int main()
{
	PRINTF("\n\n\t *** NNTOOL KWS Example ***\n\n");

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
        PRINTF("Usage: mnist [image_file]\n");
        exit(-1);
    }
    WavName = argv[1];
    PRINTF("\n\n\t *** IMAGENET EMUL ***\n\n");
    kws_ds_cnn();
    return 0;
}
#endif