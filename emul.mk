# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

DUMP_TENSORS ?= 1
SMALL  ?= 0
MEDIUM ?= 1
LARGE  ?= 0
ifeq ($(SMALL), 1)
	MODEL_PREFIX = KWS_ds_cnn_s_quant
	AT_INPUT_WIDTH=10
	AT_INPUT_HEIGHT=49
	CFLAGS += -DSMALL
else
	ifeq ($(MEDIUM), 1)
		MODEL_PREFIX = KWS_ds_cnn_m_quant
		AT_INPUT_WIDTH=10
		AT_INPUT_HEIGHT=49
		CFLAGS += -DMEDIUM
	else
		ifeq ($(LARGE), 1)
			MODEL_PREFIX = KWS_ds_cnn_l_quant
			AT_INPUT_WIDTH=40
			AT_INPUT_HEIGHT=98
			CFLAGS += -DLARGE
		else
$(error You must set to 1 one of SMALL, MEDIUM, LARGE to select a network)
		endif
	endif
endif
MODEL_SQ8=1
RM=rm -f

IMAGE = $(CURDIR)/samples/yes_features_int8.pgm

QUANT_BITS=8
BUILD_DIR=BUILD

NNTOOL_SCRIPT_PARAMETRIC=model/nntool_script_params
ifeq ($(DUMP_TENOSRS), 1)
	NNTOOL_SET_GRAPH_DUMP = set graph_dump_tensor 7
endif
NNTOOL_EXTRA_FLAGS = -q
MODEL_SUFFIX = _$(QUANT_BITS)BIT_EMUL

CLUSTER_STACK_SIZE=4096
CLUSTER_SLAVE_STACK_SIZE=1024
TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=350000
MODEL_L3_MEMORY=6388608
MODEL_SIZE_CFLAGS = -DAT_INPUT_HEIGHT=$(AT_INPUT_HEIGHT) -DAT_INPUT_WIDTH=$(AT_INPUT_WIDTH) -DAT_INPUT_COLORS=$(AT_INPUT_COLORS)

include common/model_decl.mk

MODEL_GEN_EXTRA_FLAGS= -f $(MODEL_BUILD)
CC = gcc
CFLAGS   += -g -O3 -D__EMUL__ $(MODEL_SIZE_CFLAGS)
INCLUDES  = -I. -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD) -I$(GAP_SDK_HOME)/libs/gap_lib/include
SRCS      = main.c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)
LFLAGS    =
LIBS      =

BUILD_DIR = BUILD_EMUL

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

MAIN = kws_ds_cnn_emul

all: model $(MAIN)

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

clean: #clean_model
	$(RM) -r $(BUILD_DIR)
	$(RM) $(MAIN)

run: $(MAIN)
	./$(MAIN) $(IMAGE)

include common/model_rules.mk
.PHONY: depend clean
