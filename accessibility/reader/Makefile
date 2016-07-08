
CXXFLAGS=-std=c++11 -g -I/usr/include/speech-dispatcher

progs=q
OBJS= \
	q.o \
	speech.o \
	ui.o \
	view.o \
	io.o \
	misc.o

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(progs)

clean:
	rm -f $(progs) $(OBJS)

q: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -lncurses -lspeechd

q.o: q.cpp speech.h description.h view.h ui.h misc.h io.h
