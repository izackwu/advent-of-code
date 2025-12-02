# Makefile for Advent of Code
#
# Usage:
#   make run DAY=01  - Fetches input, compiles, and runs the solution for the given day.
#   make clean       - Removes all generated files (executables and inputs).

# --- Configuration ---
# Default day if not provided
DAY  ?= 01
YEAR ?= 2025
# C++ compiler and flags
CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -O2 -fsanitize=address,undefined -fno-sanitize-recover=all

# --- File and Directory Definitions ---
DIR         := day-$(DAY)
SOURCE      := $(DIR)/main.cpp
EXECUTABLE  := $(DIR)/main.out
INPUT       := $(DIR)/input.txt

# --- Environment ---
# Load the session cookie from the .env file.
# The '-' before 'include' suppresses errors if the file doesn't exist.
-include .env

# --- Targets ---

# Phony targets are commands that don't represent files.
.PHONY: all run input clean

# Default target when running `make`
all: run

# Run the solution for the specified day.
# This depends on having the executable and the input file ready.
run: $(EXECUTABLE) $(INPUT)
	@echo "--- Running Day $(DAY) ---"
	@./$(EXECUTABLE) < $(INPUT)
	@echo "------------------------"

# Compile the source code.
# This rule runs only if main.cpp is newer than main.out or if main.out doesn't exist.
$(EXECUTABLE): $(SOURCE)
	@echo "Compiling $(SOURCE)..."
	@$(CXX) $(CXXFLAGS) -o $@ $<

# Fetch the puzzle input.
# This rule runs only if the input file doesn't exist.
# It requires the AOC_SESSION variable from the .env file.
$(INPUT):
	@if [ -z "$(AOC_SESSION)" ]; then \
		echo "Error: AOC_SESSION is not set in your .env file."; \
		echo "Please paste your session cookie into the .env file."; \
		exit 1; \
	fi
	@echo "Fetching input for Year $(YEAR), Day $(DAY)..."
	@curl -s --fail --cookie "session=$(AOC_SESSION)" \
		"https://adventofcode.com/$(YEAR)/day/$(shell echo $(DAY) | sed 's/^0*//')/input" -o $@ || \
		(echo "Error: Failed to fetch input. Your session cookie may be invalid or expired." && rm -f $@ && exit 1)


# Clean up all generated files.
clean:
	@echo "Cleaning up generated files..."
	@rm -f day-*/main.out day-*/input.txt

# Prevent make from deleting the input file after compilation
.SECONDARY: $(INPUT)
