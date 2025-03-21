# Define the compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Define the target executable
TARGET = hw2

# Define the input and expected output directories
INPUT_DIR = tests
EXPECTED_OUTPUT_DIR = expected_results

# Define the input and expected output files
INPUT_FILES = $(wildcard $(INPUT_DIR)/*.txt)
EXPECTED_OUTPUT_FILES = $(patsubst $(INPUT_DIR)/%.txt,$(EXPECTED_OUTPUT_DIR)/%.txt,$(notdir $(INPUT_FILES)))

# Compile the program
$(TARGET): hw2.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) hw2.cpp

# Test target
.PHONY: test
test: $(TARGET)
    @for input_file in $(INPUT_FILES); do \
        output_file=$(notdir $$input_file | sed 's/.txt/.out/'); \
        ./$(TARGET) < $$input_file > $$output_file; \
        diff $$output_file $(EXPECTED_OUTPUT_DIR)/$$(basename $$output_file) || { \
            echo "Test failed for $$input_file"; exit 1; \
        }; \
        rm $$output_file; \
    done; \
    echo "All tests passed."

# Clean target
.PHONY: clean
clean:
	rm -f $(TARGET)