CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Ilibs/libsodium/include 
LDFLAGS = -Llibs/libsodium/lib -lsodium -lws2_32

SRC = $(wildcard src/*.c)
OUT = build/bvault.exe

all: $(OUT)

$(OUT): $(SRC)
	@if not exist build mkdir build
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(OUT)

clean:
	if exist build rmdir /s /q build

run : 
	$(OUT) 