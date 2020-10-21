# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

SMALL  ?= 0
MEDIUM ?= 0
LARGE  ?= 0
ifeq ($(SMALL), 1)
	MODEL_PREFIX = KWS_ds_cnn_s_quant
	AT_INPUT_WIDTH=10
	AT_INPUT_HEIGHT=49
	APP_CFLAGS += -DSMALL
else
	ifeq ($(MEDIUM), 1)
		MODEL_PREFIX = KWS_ds_cnn_m_quant
		AT_INPUT_WIDTH=10
		AT_INPUT_HEIGHT=49
		APP_CFLAGS += -DMEDIUM
	else
	ifeq ($(LARGE), 1)
		MODEL_PREFIX = KWS_ds_cnn_l_quant
		AT_INPUT_WIDTH=40
		AT_INPUT_HEIGHT=98
		APP_CFLAGS += -DLARGE
	endif
endif
endif
MODEL_SQ8=1
pulpChip = GAP
RM=rm -f

IMAGE = $(CURDIR)/samples/on_sample_features_int8.pgm

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

APP_SRCS += main.c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)

APP_CFLAGS += -O3 -s -mno-memcpy -fno-tree-loop-distribute-patterns 
APP_CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(MODEL_BUILD) $(CNN_LIB_INCLUDE)
APP_CFLAGS += -DPERF -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS)
APP_CFLAGS += -DSTACK_SIZE=$(CLUSTER_STACK_SIZE) -DSLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE)
APP_CFLAGS += -DAT_IMAGE=$(IMAGE)

# all depends on the model
all:: model

clean:: clean_at_model

clean_at_model:
	$(RM) $(MODEL_GEN_EXE)

at_model_disp:: $(MODEL_BUILD) $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS) --debug=Disp

at_model:: $(MODEL_BUILD) $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS)

include common/model_rules.mk

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
