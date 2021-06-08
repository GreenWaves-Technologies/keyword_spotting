# User Test
#------------------------------------------
MFCC_GENERATOR ?= $(TILER_GENERATOR_PATH)/MFCC
MFCCBUILD_DIR ?= $(CURDIR)/BUILD_MFCC_MODEL
MFCC_MODEL_GEN = $(MFCCBUILD_DIR)/GenMFCC
MFCC_SRCG += $(MFCC_GENERATOR)/MfccGenerator.c
MFCC_KER_SRCS = $(MFCC_GENERATOR)/MfccBasicKernels.c $(MFCC_GENERATOR)/FFTLib.c 

# Everything bellow is not application specific
TABLE_CFLAGS=-lm

#SDL_FLAGS= -lSDL2 -lSDL2_ttf -DAT_DISPLAY
ifdef MODEL_L1_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L1 $(MODEL_L1_MEMORY)
endif
ifdef MODEL_L2_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L2 $(MODEL_L2_MEMORY)
endif
ifdef MODEL_L3_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L3 $(MODEL_L3_MEMORY)
endif

SMALL  ?= 0
MEDIUM ?= 0
LARGE  ?= 0
ifeq ($(SMALL), 1)
SIZE_DEF += -DSMALL
NN_SIZE = SMALL
else
ifeq ($(MEDIUM), 1)
SIZE_DEF += -DMEDIUM
NN_SIZE = MEDIUM
else
ifeq ($(LARGE), 1)
SIZE_DEF += -DLARGE
NN_SIZE = LARGE
else
$(error You must set to 1 one of SMALL, MEDIUM, LARGE to select a network)
endif
endif
endif

EXTRA_FLAGS =
ifeq ($(USE_POWER), 1)
	EXTRA_FLAGS += --use_power
endif
ifeq ($(USE_HIGH_PREC), 1)
	EXTRA_FLAGS += --use_high_prec
else ifeq ($(FIX32), 1)
	EXTRA_ARGS += --use_high_prec
endif

$(MFCCBUILD_DIR):
	mkdir $(MFCCBUILD_DIR)

$(MFCCBUILD_DIR)/LUT.def: $(MFCCBUILD_DIR)
	python3 $(MFCC_GENERATOR)/GenLUT.py --fft_lut_file $(MFCCBUILD_DIR)/LUT.def --mfcc_bf_lut_file $(MFCCBUILD_DIR)/MFCC_FB.def                \
									   --sample_rate 16000 --frame_size $(FRAME_SIZE) --frame_step $(FRAME_STEP) --n_frame $(AT_INPUT_HEIGHT) \
									   --n_fft 1024 --n_dct 40 --mfcc_bank_cnt 40 --fmin 20 --fmax 4000 --mfcc_bank_cnt 40 --preempfactor 0.0 \
									   --use_tf_mfcc --save_params_header MFCC_params_$(NN_SIZE).h $(EXTRA_FLAGS)

# Build the code generator from the model code
$(MFCC_MODEL_GEN): $(MFCCBUILD_DIR)/LUT.def $(MFCCBUILD_DIR)
	gcc -g -o $(MFCC_MODEL_GEN) -I. -I$(MFCC_GENERATOR) -I$(TILER_INC) -I$(TILER_EMU_INC) MFCCmodel.c $(MFCC_SRCG) $(TILER_LIB) $(TABLE_CFLAGS) $(SIZE_DEF)


# Run the code generator  kernel code
$(MFCCBUILD_DIR)/MFCCKernels.c: $(MFCC_MODEL_GEN)
	$(MFCC_MODEL_GEN) -o $(MFCCBUILD_DIR) -c $(MFCCBUILD_DIR) $(MODEL_GEN_EXTRA_FLAGS)

mfcc_model: $(MFCCBUILD_DIR)/MFCCKernels.c

clean_mfcc_model:
	rm -rf $(MFCCBUILD_DIR)
