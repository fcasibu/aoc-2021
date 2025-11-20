CC = gcc
INCLUDE_LIBS = -I./include
CFLAGS = -std=c2x -Wall -Wextra -Werror -Wpedantic -g -O2

BUILD_DIR = build

run:
	@if [ -z "$(day)" ]; then \
		echo "Usage: make run day=dayXXX"; \
		exit 1; \
	fi
	$(MAKE) $(BUILD_DIR)/$(day)/main
	$(BUILD_DIR)/$(day)/main

$(BUILD_DIR)/%/main: %/main.c
	mkdir -p $(BUILD_DIR)/$*
	$(CC) $(CFLAGS) $(INCLUDE_LIBS) $< -o $@

.PHONY: run
