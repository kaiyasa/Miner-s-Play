
TARGET       := q
TARGET_DIR   := build/dist

CXXFLAGS     := -std=c++11 -pthread -g
SOURCES      := $(shell find src -name '*.cpp')
TGT_LDFLAGS  := -pthread
TGT_LDLIBS   := -lncurses

SRC_INCDIRS  := /usr/include/speech-dispatcher \
	include
