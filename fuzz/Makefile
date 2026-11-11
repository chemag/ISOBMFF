# Variables
SRC_DIR = ../test
BUILD_DIR = ../build/fuzz
CORPUS_DIR = corpus
CONVERTER = ./converter.py
RUNS ?= 100

# List of fuzzers: one per unit test carrying fuzzer::conv markers
FUZZERS = $(patsubst $(SRC_DIR)/%_unittest.cpp,%,$(shell grep -l 'fuzzer::conv: begin' $(SRC_DIR)/*_unittest.cpp))

# Default target
all: $(addsuffix _fuzzer.cpp, $(FUZZERS))
	@echo "run: RUNS=$(RUNS) make -j -Oline fuzz"

# Pattern rule for generating fuzzer source files
%_fuzzer.cpp: $(SRC_DIR)/%_unittest.cpp
	$(CONVERTER) $< ./

# Fuzz target
fuzz: $(addsuffix _fuzzer, $(FUZZERS))

# Pattern rule for running fuzzers
%_fuzzer: %_fuzzer.cpp
	-$(BUILD_DIR)/$*_fuzzer -artifact_prefix=$(CORPUS_DIR)/$*_fuzzer/ $(CORPUS_DIR)/$*_fuzzer/ -runs=$(RUNS)
