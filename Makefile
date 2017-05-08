SOLVER_USE_SOPLEX = true
SOPLEXPATH =../../lib/soplex-3.0.0/lib/
SOPLEXINCLUDE =../../lib/soplex-3.0.0/src/

SOLVER_USE_ARMADILLO = true
ARMADILLOPATH =../../lib/armadillo-7.800.2/lib/usr/lib
ARMADILLOINCLUDE =../../lib/armadillo-7.800.2/lib/usr/include

CC = g++
CPPFLAGS = -g -std=c++0x -O3 
#CPPFLAGS += -DNDEBUG
CPPFLAGS += -pg -ggdb
CPPFLAGS += -Wall -Werror=return-type


ifeq ($(SOLVER_USE_SOPLEX),true)
	CPPFLAGS += -DSOLVER_USE_SOPLEX
	CPPFLAGS += -I $(SOPLEXINCLUDE)
	LIBS += -L $(SOPLEXPATH)  -lsoplex -lz -lgmp 	
endif
ifeq ($(SOLVER_USE_ARMADILLO),true)
	CPPFLAGS += -DSOLVER_USE_ARMADILLO
	CPPFLAGS += -I $(ARMADILLOINCLUDE)
	LIBS += -L $(ARMADILLOPATH)  -larmadillo -llapack 	
endif

SRCS = $(wildcard ./src/core/*.cpp) 
OBJS = $(addsuffix .o, $(basename $(SRCS)))
DEPS = $(addsuffix .d, $(basename $(SRCS)))

all : kea

kea : $(OBJS)
	$(CC) $(CPPFLAGS) -o bin/kea-core $(OBJS) $(LIBS)

#%.o : %.c
#	$(CC) $(CPPFLAGS) $(LIBS) -o bin/kea-core $(OBJS)

clean :
	rm -f bin/kea-core $(OBJS) $(DEPS) *~

-include $(DEPS)

%.d: %.c
	@$(CC) -MM -MT  $(subst .d,.o,$@) -MT $@ $(CPPFLAGS) $< > $@
