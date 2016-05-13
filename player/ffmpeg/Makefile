
PROGS=tutorial01 tutorial03 tutorial04 tutorial05

AVLIBS=-lavutil -lavformat -lavcodec -lswscale -lz
SDLFLAGS=`sdl-config --cflags`
SDLLIBS=`sdl-config --libs`

all: $(PROGS)

clean:
	rm $(PROGS)

tutorial01:
	$(CC) -o $@ $@.c $(AVLIBS)

tutorial03:
	$(CC) -o $@ $@.c $(AVLIBS) $(SDLFLAGS) $(SDLLIBS)

tutorial04:
	$(CC) -o $@ $@.c $(AVLIBS) $(SDLFLAGS) $(SDLLIBS) -lm

tutorial05:
	$(CC) -o $@ $@.c $(AVLIBS) $(SDLFLAGS) $(SDLLIBS) -lm
