CC=gcc
BIN_NAME=overlay
OUT_DIR=./build

overlay: setup
	$(CC) -o $(OUT_DIR)/$(BIN_NAME) -g overlay/main.c overlay/overlay.c -lX11

setup:
	if [ ! -d $(OUT_DIR) ]; then mkdir $(OUT_DIR); fi

clean:
	if [ -d $(OUT_DIR) ]; then rm -rf $(OUT_DIR); fi
