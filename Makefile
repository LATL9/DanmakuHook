DBGEXE:=DanmakuHook-debug
RELEXE:=DanmakuHook-release
CONFIG:=debug
OS?=LIN

LDFLAGS:= -L libtorch/lib -ltorch -lcaffe2 -lc10

CFLAGS:= -pedantic -Wall -Wextra -Wno-missing-field-initializers -std=c++17 -I include -I libtorch/include -I libtorch/include/torch/csrc/api/include

DBGBINDIR:=bin/debug
RELBINDIR:=bin/release
DBGOBJDIR:=obj/debug
RELOBJDIR:=obj/release
CC=g++

SRC:=$(wildcard src/*.cpp)
INC:=$(wildcard include/*.hpp)
DBGOBJ:=$(SRC:src/%.cpp=$(DBGOBJDIR)/%.o)
RELOBJ:=$(SRC:src/%.cpp=$(RELOBJDIR)/%.o)

.PHONY: all debug release clean style

all: debug release

debug:CONFIG:=debug
debug:CFLAGS+= -g

release:CONFIG:=release
release: CFLAGS+= -O3

debug: $(DBGOBJ)
	$(CC) $(DBGOBJ) -o $(DBGBINDIR)/$(DBGEXE) $(LDFLAGS)

release: $(RELOBJ)
	$(CC) $(RELOBJ) -o  $(RELBINDIR)/$(RELEXE) $(LDFLAGS)

$(DBGOBJDIR)/%.o : src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(RELOBJDIR)/%.o : src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm obj/debug/* obj/release/* obj/debug/* obj/release/* bin/debug/$(DBGEXE) bin/release/$(RELEXE) bin/debug/$(DBGEXE) bin/release/$(RELEXE) -f
