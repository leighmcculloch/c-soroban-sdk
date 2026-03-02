CONTRACT = increment
SDK_DIR  = ../../sdk

CC       = clang
CFLAGS   = --target=wasm32 --no-standard-libraries -std=c99 -O2 -I$(SDK_DIR)
LDFLAGS  = -Wl,--no-entry -Wl,--allow-undefined -Wl,--strip-all -Wl,--export-dynamic

$(CONTRACT).wasm: $(CONTRACT).o $(CONTRACT).sections.o
	$(CC) --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--allow-undefined -Wl,--strip-all -Wl,--export-dynamic -o $@ $^

$(CONTRACT).o: $(CONTRACT).c $(SDK_DIR)/soroban.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(CONTRACT).sections.o: $(CONTRACT).S $(SDK_DIR)/soroban.h
	$(CC) --target=wasm32 -I$(SDK_DIR) -c -o $@ $<

clean:
	rm -f $(CONTRACT).wasm $(CONTRACT).o $(CONTRACT).sections.o

.PHONY: clean
