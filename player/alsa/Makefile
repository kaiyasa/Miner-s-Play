
CXXFLAGS=-g

progs=exp1 exp2


all: $(progs)

clean:
	rm -f $(progs)

exp1: exp1.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lasound

exp2: exp2.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lasound
