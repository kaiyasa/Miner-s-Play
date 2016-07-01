
CXXFLAGS=-std=c++11 -g -I/usr/include/speech-dispatcher

progs=q

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(progs)

clean:
	rm -f $(progs) *.o

q: q.o
	$(CXX) $(CXXFLAGS) -o $@ $< -lncurses -lspeechd
