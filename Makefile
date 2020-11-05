# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

WITH_MFCC ?= 0
SMALL  ?= 0
MEDIUM ?= 0
LARGE  ?= 0
ifeq ($(SMALL), 1)
	MODEL_PREFIX = KWS_ds_cnn_s_quant
	DCT_COUNT = 10
	FRAME_SIZE_ms = 40
	FRAME_STEP_ms = 20
	AT_INPUT_WIDTH=10
	AT_INPUT_HEIGHT=49
	APP_CFLAGS += -DSMALL
else
	ifeq ($(MEDIUM), 1)
		MODEL_PREFIX = KWS_ds_cnn_m_quant
		DCT_COUNT = 10
		FRAME_SIZE_ms = 40
		FRAME_STEP_ms = 20
		AT_INPUT_WIDTH=10
		AT_INPUT_HEIGHT=49
		APP_CFLAGS += -DMEDIUM
	else
		ifeq ($(LARGE), 1)
			MODEL_PREFIX = KWS_ds_cnn_l_quant
			DCT_COUNT = 40
			FRAME_SIZE_ms = 30
			FRAME_STEP_ms = 10
			AT_INPUT_WIDTH=40
			AT_INPUT_HEIGHT=98
			APP_CFLAGS += -DLARGE
		else
$(error You must set to 1 one of SMALL, MEDIUM, LARGE to select a network)
		endif
	endif
endif
MODEL_SQ8=1
pulpChip = GAP
RM=rm -f

IMAGE = $(CURDIR)/samples/on_sample_features_int8.pgm
WAV_PATH = $(CURDIR)/samples/on_sample.wav

READFS_FILES=$(realpath $(MODEL_TENSORS))

QUANT_BITS=8
BUILD_DIR=BUILD

NNTOOL_SCRIPT_PARAMETRIC=model/nntool_script_params
NNTOOL_EXTRA_FLAGS = -q
MODEL_SUFFIX = _$(QUANT_BITS)BIT
MODEL_BUILD = BUILD_MODEL_$(QUANT_BITS)BIT

CLUSTER_STACK_SIZE=4096
CLUSTER_SLAVE_STACK_SIZE=1024
TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=350000
MODEL_L3_MEMORY=8000000
MODEL_SIZE_CFLAGS = -DAT_INPUT_HEIGHT=$(AT_INPUT_HEIGHT) -DAT_INPUT_WIDTH=$(AT_INPUT_WIDTH) -DAT_INPUT_COLORS=$(AT_INPUT_COLORS)

include common/model_decl.mk

ifeq ($(WITH_MFCC), 1)
	APP_SRCS    += main_with_mfcc.c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)
	APP_SRCS    += common/wavIO.c $(MFCCBUILD_DIR)/MFCCKernels.c $(MFCC_DIR)/MfccBasicKernels.c $(MFCC_DIR)/FFTLib.c
	APP_CFLAGS  += -Icommon -I$(MFCC_DIR) -I$(MFCCBUILD_DIR) -I$(LUT_GEN_DIR)
else
	APP_SRCS    += main.c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)
endif

APP_CFLAGS += -O3 -s -mno-memcpy -fno-tree-loop-distribute-patterns 
APP_CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(MODEL_BUILD) $(CNN_LIB_INCLUDE)
APP_CFLAGS += -Icommon -I$(MFCC_DIR) -I$(MFCCBUILD_DIR) -I$(LUT_GEN_DIR)
APP_CFLAGS += -DPERF -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS)
APP_CFLAGS += -DSTACK_SIZE=$(CLUSTER_STACK_SIZE) -DSLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE)
APP_CFLAGS += -DAT_IMAGE=$(IMAGE) -DAT_WAV=$(WAV_PATH) #-DPRINT_WAV -DPRINT_AT_INPUT

generate_samples:
	python utils/generate_samples_images.py --dct_coefficient_count $(DCT_COUNT) --window_size_ms $(FRAME_SIZE_ms) --window_stride_ms $(FRAME_STEP_ms)

test_accuracy:
	python utils/test_accuracy_emul.py --tflite_model $(TRAINED_TFLITE_MODEL) --dct_coefficient_count $(DCT_COUNT) --window_size_ms $(FRAME_SIZE_ms) --window_stride_ms $(FRAME_STEP_ms) --test_with_wav $(WITH_MFCC)

test_accuracy_tflite:
	python utils/test_accuracy_tflite.py --tflite_model $(TRAINED_TFLITE_MODEL) --dct_coefficient_count $(DCT_COUNT) --window_size_ms $(FRAME_SIZE_ms) --window_stride_ms $(FRAME_STEP_ms)

# all depends on the model
all:: model mfcc_model generate_samples

clean:: clean_at_model clean_mfcc_model

clean_at_model:
	$(RM) $(MODEL_GEN_EXE)

at_model_disp:: $(MODEL_BUILD) $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS) --debug=Disp

at_model:: $(MODEL_BUILD) $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS)

include common/model_rules.mk
include mfcc_model.mk

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
