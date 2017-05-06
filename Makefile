SOPLEXPATH = ~/lib/soplex-3.0.0/lib/
SOPLEXINCLUDE = ~/lib/soplex-3.0.0/src
SOPLEXDEP = ~/lib/soplex-3.0.0/src

CC = g++
CPPFLAGS = -g -std=c++0x -O3 -I ./src/core  
CPPFLAGS += -Wall -Werror=return-type -Wno-non-virtual-dtor -Wno-unused-function -Wno-deprecated
#CPPFLAGS += -DNDEBUG
CPPFLAGS += -pg -ggdb
CPPFLAGS += -L $(SOPLEXPATH) -lz -lgmp -lsoplex

SRCS = $(wildcard ./src/core/*.cpp) 
OBJS = $(addsuffix .o, $(basename $(SRCS)))
DEPS = $(addsuffix .d, $(basename $(SRCS)))

all : kea

kea : $(OBJS)
	$(CC) $(CPPFLAGS) -o bin/kea-core $(OBJS)

clean :
	rm -f bin/kea-core $(OBJS) $(DEPS) *~

-include $(DEPS)

%.d: %.c
	@$(CC) -MM -MT  $(subst .d,.o,$@) -MT $@ $(CPPFLAGS) $< > $@
