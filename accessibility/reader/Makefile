
progs=hello chello keyb win q

all: $(progs) win q

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

q: q.cpp
	$(CXX) -std=c++11 -g -o $@ $< -lncurses -lspeechd
