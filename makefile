# Simple Makefile for cMusix

CC = gcc
SRC = main.c
OUT_DIR = bin
OUT = $(OUT_DIR)/cmusix

.PHONY: all run clean

all: $(OUT)

$(OUT): $(SRC)
	@mkdir -p $(OUT_DIR)
	$(CC) $(SRC) -o $(OUT)
	@echo "Built cmusix -> $(OUT)"

run: all
	@echo "Running cmusix..."
	./$(OUT)

clean:
	rm -f $(OUT)
	@echo "Cleaned up!"
