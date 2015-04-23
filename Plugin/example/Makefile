#
# Toolbox/Plugin Example
#

TARGET=example

SRC_DIR=src
OBJ_DIR=obj
PLUGIN_DIR=plugin
PLUGIN_SRC_DIR=$(SRC_DIR)/$(PLUGIN_DIR)

CPP_EXT=cpp
OBJ_EXT=o
PLUGIN_EXT=so

# One for each application cpp file in the $(SRC_DIR)
OBJ=$(OBJ_DIR)/main.$(OBJ_EXT)

# One for each plugin cpp file in the $(PLUGIN_SRC_DIR)
PLUGINS=$(PLUGIN_DIR)/simple1.$(PLUGIN_EXT)	\
	$(PLUGIN_DIR)/simple2.$(PLUGIN_EXT)

CPP=g++
C_FLAGS=-std=c++14 -Wall -pedantic
LD_FLAGS=
PLUGIN_LD_FLAGS=-fPIC -shared
LIBS=-ldl


all: $(TARGET) plugins

$(OBJ_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.$(CPP_EXT) $(OBJ_DIR)
	$(CPP) $(C_FLAGS) -c -o $@ $<

$(TARGET): $(OBJ)
	$(CPP) $(LD_FLAGS) $(LIBS) -o $@ $<

$(PLUGIN_DIR)/%.$(PLUGIN_EXT): $(PLUGIN_SRC_DIR)/%.$(CPP_EXT) $(PLUGIN_DIR)
	$(CPP) $(C_FLAGS) -o $@ $< $(PLUGIN_LD_FLAGS)

plugins: $(PLUGINS)

$(OBJ_DIR):
	@echo Creating object file directory \'$(OBJ_DIR)\'
	@mkdir $(OBJ_DIR)

$(PLUGIN_DIR):
	@echo Creating plugin directory \'$(PLUGIN_DIR)\'
	@mkdir $(PLUGIN_DIR)

clean:
	@echo Cleaning all generated files.
	@rm -rf $(OBJ_DIR) $(PLUGIN_DIR) $(TARGET)

fresh: clean all


