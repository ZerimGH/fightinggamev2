TARGET_NAME = main
OS ?= linux

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O3

BUILD_DIR_NAME = build
BUILD_DIR = $(BUILD_DIR_NAME)/$(OS)
OBJ_DIR = $(BUILD_DIR)/obj

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$(d),$(2)) $(filter $(subst *,%,$(2)),$(d)))
SRCS = $(call rwildcard,src,*.c)
ALL_SRC_DIRS = $(sort $(dir $(call rwildcard,src,*)))
INCLUDE_DIRS = $(ALL_SRC_DIRS) extern/include
CFLAGS += $(addprefix -I,$(INCLUDE_DIRS))

OBJS = $(patsubst src/%, $(OBJ_DIR)/%, $(SRCS:.c=.o))

LD = $(CC)

ifeq ($(OS), win)
	TARGET = $(BUILD_DIR)/$(TARGET_NAME).exe
	LDFLAGS = -Lextern/lib/win -lraylib -lenet -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32 -mwindows
else ifeq ($(OS), macos)
	TARGET = $(BUILD_DIR)/$(TARGET_NAME)
	LDFLAGS = -Lextern/lib/macos -lraylib -lenet -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else
	TARGET = $(BUILD_DIR)/$(TARGET_NAME)
	LDFLAGS = -Lextern/lib/linux -lraylib -lenet -lm -ldl -lGL -pthread
endif

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean: 
	rm -rf $(BUILD_DIR_NAME)

.PHONY: run
run: all 
	$(TARGET)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(LD) $(OBJS) $(LDFLAGS) -o $(TARGET)
