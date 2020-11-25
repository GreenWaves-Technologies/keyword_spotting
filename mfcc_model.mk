# User Test
#------------------------------------------
MFCC_DIR ?= $(CURDIR)/MFCC_GENERATORS
MFCCBUILD_DIR ?= $(CURDIR)/BUILD_MFCC_MODEL
MFCC_MODEL_GEN = $(MFCCBUILD_DIR)/GenMFCC
LUT_GEN_DIR = $(MFCC_DIR)/lut_generators
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
else
ifeq ($(MEDIUM), 1)
SIZE_DEF += -DMEDIUM
else
ifeq ($(LARGE), 1)
SIZE_DEF += -DLARGE
else
$(error You must set to 1 one of SMALL, MEDIUM, LARGE to select a network)
endif
endif
endif

$(MFCCBUILD_DIR):
	mkdir $(MFCCBUILD_DIR)

# Build the code generator from the model code
$(MFCC_MODEL_GEN): $(MFCCBUILD_DIR)
	gcc -g -o $(MFCC_MODEL_GEN) -I$(MFCC_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) $(SIZE_DEF) \
			  $(MFCC_DIR)/MFCCmodel.c $(MFCC_SRCG) $(TILER_LIB)  $(TABLE_CFLAGS) #$(SDL_FLAGS)

gen_lut:
	python $(LUT_GEN_DIR)/gen_lut.py

# Run the code generator  kernel code
mfcc_model: $(MFCC_MODEL_GEN) gen_lut
	$(MFCC_MODEL_GEN) -o $(MFCCBUILD_DIR) -c $(MFCCBUILD_DIR) $(MODEL_GEN_EXTRA_FLAGS)

clean_mfcc_model:
	rm -rf $(MFCCBUILD_DIR)
