
progs=hello chello keyb win

all: $(progs) win

clean:
	rm -f $(progs)

hello: hello.c
	$(CC) -o $@ $< -lncurses

chello: chello.c
	$(CC) -o $@ $< -lncurses

keyb: keyb.c
	$(CC) -o $@ $< -lncurses

win: win.c
	$(CC) -o $@ $< -lncurses
