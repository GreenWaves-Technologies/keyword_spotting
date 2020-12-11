# User Test
#------------------------------------------
MFCC_DIR ?= $(CURDIR)/MFCC
MFCCBUILD_DIR ?= $(CURDIR)/BUILD_MFCC_MODEL
MFCC_MODEL_GEN = $(MFCCBUILD_DIR)/GenMFCC
LUT_GEN_DIR = $(MFCC_DIR)
LUTS = $(LUT_GEN_DIR)/MFCC_FB.def $(LUT_GEN_DIR)/LUT.def
MFCC_SRCG += $(MFCC_DIR)/MFCC_Generator.c

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
PARAMS = $(MFCC_DIR)/MFCC_params_$(NN_SIZE).h $(LUTS)

ifeq ($(USE_HIGH_PREC), 1)
	EXTRA_DEF = -DUSE_HIGH_PREC=1
else
	EXTRA_DEF = -DUSE_HIGH_PREC=0
endif
ifeq ($(USE_POWER), 1)
	EXTRA_DEF += -DUSE_POWER=1
else
	EXTRA_DEF += -DUSE_POWER=0
endif

$(MFCCBUILD_DIR):
	mkdir $(MFCCBUILD_DIR)

# Build the code generator from the model code
$(MFCC_MODEL_GEN): $(MFCCBUILD_DIR)
	gcc -g -o $(MFCC_MODEL_GEN) $(EXTRA_DEF) -I$(MFCC_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) $(SIZE_DEF) $(MFCC_DIR)/MFCCmodel.c $(MFCC_SRCG) $(TILER_LIB)  $(TABLE_CFLAGS) #$(SDL_FLAGS)

$(PARAMS): $(MFCCBUILD_DIR)
	python3 $(LUT_GEN_DIR)/gen_lut.py $(NN_SIZE)

# Run the code generator  kernel code
$(MFCCBUILD_DIR)/MFCCKernels.c: $(PARAMS) $(MFCC_MODEL_GEN) $()
	$(MFCC_MODEL_GEN) -o $(MFCCBUILD_DIR) -c $(MFCCBUILD_DIR) $(MODEL_GEN_EXTRA_FLAGS)


mfcc_model: $(MFCCBUILD_DIR)/MFCCKernels.c

clean_mfcc_model:
	rm -rf $(MFCCBUILD_DIR)
