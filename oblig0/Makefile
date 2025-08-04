# Authors: Odin Bjerke <odin.bjerke@uit.no>
# Modified by: Morten Grønnesby <morten.gronnesby@uit.no>

# Commands
CC = gcc
EXE = wordfreq
DEBUG = 1

# Directories
SRC_DIR = src
OBJ_DIR = obj
INCLUDE = include

# Source and object files
SRC := $(wildcard src/*.c)
HEADERS := $(wildcard $(INCLUDE)/*.h)
OBJ := $(patsubst src/%.c,obj/%.o,$(SRC))


# linked libraries
LDFLAGS += -lm

CFLAGS += -std=c2x

# options for printing.h
CFLAGS += -D LOG_LEVEL=LOG_LEVEL_DEBUG
# CFLAGS += -D PRINTING_NCOLOR
# CFLAGS += -D PRINTING_NMETA

# Turn off debugprints and utilize highest optimization level
ifeq ($(DEBUG), 0)
CFLAGS += -O3 -DNDEBUG
BUILD_DIR := bin/release
TARGET := $(BUILD_DIR)/$(EXE)
else
CFLAGS += -Og -DDEBUG -D_GNU_SOURCE -g -Wall -Wextra
BUILD_DIR := bin/debug
TARGET := $(BUILD_DIR)/$(EXE)
endif


.PHONY: all exec
.PHONY: clean distclean
.PHONY: dirs

all: dirs exec
exec: $(TARGET)

$(TARGET): $(OBJ) $(HEADERS) Makefile
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

dirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(OBJ)
	rm -rf bin/debug
	rm -rf bin/release

distclean: clean
	rm -rf $(OBJ_DIR)
	rm -rf .DS_Store
	rm -rf bin/


