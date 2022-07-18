src = main.c
obj = main.o

LDFLAGS = -lSDL2
EMFLAGS = -O2 -s USE_SDL=2 -s SINGLE_FILE --preload-file fendemo_star.bmp --preload-file techdemo.bmp --preload-file techdemo_text.bmp

fendemo: main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

fendemo-web: main.c
	emcc $(EMFLAGS) -o fendemo.html $^

.PHONY: clean
clean:
	rm -f $(obj) fendemo fendemo.data fendemo.js fendemo.wasm fendemo.html