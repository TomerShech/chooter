PROGRAM_NAME := chooter

SRC_DIR := src
INC_DIR := inc
OBJ_DIR := obj
BIN_DIR := bin

EXE := $(BIN_DIR)/$(PROGRAM_NAME)
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

SDL_CFLAGS  := $(shell pkg-config --cflags sdl2 SDL2_mixer SDL2_image)
SDL_LDFLAGS := $(shell pkg-config --libs sdl2 SDL2_mixer SDL2_image)

# CPP here stands for C preprocessor and not "C++"
CPPFLAGS := -I$(INC_DIR)
CFLAGS   := -Wall -Wextra -pedantic -std=c99 $(SDL_CFLAGS) 
LDFLAGS  := $(SDL_LDFLAGS)

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

clean:
	@-$(RM) -rfv $(BIN_DIR) $(OBJ_DIR)
