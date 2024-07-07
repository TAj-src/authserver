BIN = authserver

BUILD_DIR = ./build

SOURCE_DIR = ./

SOURCE = \
	authserver.c \

INCLUDES = \

CC = gcc

CC_FLAGS = -g -O2
LNK_FLAGS = -lcurl

C_OBJ = $(SOURCE:%.c=$(BUILD_DIR)/%.o)
OBJ = $(C_OBJ)


DEP = $(OBJ:%.o=%.d)
INCLUDEPATHS = $(INCLUDES:%= -I %)

$(BIN) : PreBuild $(OBJ)
	echo Linking...
	-mkdir -p $(@D) 2>/dev/null
	$(CC) $(OBJ) $(LNK_FLAGS) -o $(BIN)

-include $(DEP)

$(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.c
	echo Compiling $(notdir $<)
	-mkdir -p $(@D) 2>/dev/null
	$(CC) $(CC_FLAGS) $(INCLUDEPATHS) -MMD -c $< -o $@


.PHONY : clean
clean:
	-rm -r $(BUILD_DIR)
	-rm $(BIN)

.PHONY: PreBuild
PreBuild:
