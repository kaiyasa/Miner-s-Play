
PRODUCT := lf
PRODUCT_LIBS := -lavformat

CXX := g++
LINKER := g++
INCDIRS := -I.
CXXFLAGS := -std=c++11 -Wall -Wextra -g

SRCFILES := $(wildcard *.cpp) 
OBJFILES := $(patsubst %.cpp,%.o,$(SRCFILES))
DEPFILES := $(patsubst %.cpp,%.d,$(SRCFILES))

.PHONY: all clean

all: $(PRODUCT)

clean:
	rm -f $(OBJFILES) $(DEPFILES) $(PRODUCT)

$(PRODUCT): $(OBJFILES)
	$(LINKER) -g $^ -o $@ $(PRODUCT_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCDIRS) -c $< -o $@

%.d: %.cpp
	$(CXX) $(INCDIRS) -MM $< | sed 's/^/$@ /' > $@

-include $(DEPFILES)
