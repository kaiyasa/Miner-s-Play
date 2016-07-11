
CXXFLAGS=-std=c++11 -g -I/usr/include/speech-dispatcher

progs=q
OBJS= \
	q.o \
	speech.o \
	ui.o \
	view.o \
	io.o \
	application.o \
	misc.o

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(progs)

clean:
	rm -f $(progs) $(OBJS)

q: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -lncurses -lspeechd

q.o: q.cpp application.h description.h io.h misc.h speech.h ui.h view.h viewport.h point.h
