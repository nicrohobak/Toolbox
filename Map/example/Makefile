#
# Genetics
#


TARGET=example

SRC_DIR=src
OBJ_DIR=obj

CPP_EXT=cpp
OBJ_EXT=o

# One for each application cpp file in the $(SRC_DIR)
OBJ=$(OBJ_DIR)/main.$(OBJ_EXT)

CPP=g++
C_FLAGS=-std=c++14 -Wall -pedantic -g
LD_FLAGS=
LIBS=


all: $(TARGET)

$(OBJ_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.$(CPP_EXT) $(OBJ_DIR)
	$(CPP) $(C_FLAGS) -c -o $@ $<

$(TARGET): $(OBJ)
	$(CPP) $(LD_FLAGS) $(LIBS) -o $@ $<

$(OBJ_DIR):
	@echo Creating object file directory \'$(OBJ_DIR)\'
	@mkdir $(OBJ_DIR)

clean:
	@echo Cleaning all generated files.
	@rm -rf $(OBJ_DIR) $(PLUGIN_DIR) $(TARGET)

fresh: clean all


