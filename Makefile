SOLVER_USE_SOPLEX = true
SOPLEXPATH =../../lib/soplex-3.0.0/lib/
SOPLEXINCLUDE =-I../../lib/soplex-3.0.0/src/

SOLVER_USE_ARMADILLO = true
ARMADILLOPATH =../../lib/armadillo-7.800.2/lib/usr/lib
ARMADILLOINCLUDE =-I../../lib/armadillo-7.800.2/lib/usr/include

SOLVER_USE_PETSC = true
PETSCINCLUDE = -I/home/diego/lib/petsc-3.7.6/include \
	-I/home/diego/lib/petsc-3.7.6/arch-linux2-c-debug/include \
	-I/usr/include/mpi/
PETSCPATH = -L/home/diego/lib/petsc-3.7.6/arch-linux2-c-debug/lib \
	-L/usr/lib/openmpi/lib -L/usr/lib/gcc/x86_64-linux-gnu/5 \
	-L/usr/lib/x86_64-linux-gnu 


CC = g++
CPPFLAGS = -g -std=c++0x -O0
#CPPFLAGS += -DNDEBUG
CPPFLAGS += -pg -ggdb
CPPFLAGS += -Wall -Werror=return-type


ifeq ($(SOLVER_USE_SOPLEX),true)
	CPPFLAGS += -DSOLVER_USE_SOPLEX
	CPPFLAGS += $(SOPLEXINCLUDE)
	LIBS += -L $(SOPLEXPATH)  -lsoplex -lz -lgmp 	
endif
ifeq ($(SOLVER_USE_ARMADILLO),true)
	CPPFLAGS += -DSOLVER_USE_ARMADILLO
	CPPFLAGS += $(ARMADILLOINCLUDE)
	LIBS += -L $(ARMADILLOPATH)  -larmadillo -llapack 
endif
ifeq ($(SOLVER_USE_PETSC),true)
	CPPFLAGS += -DSOLVER_USE_PETSC
	CPPFLAGS += $(PETSCINCLUDE)
	LIBS += $(PETSCPATH) -lpetsc -lmpi_usempif08 \
	-lmpi_usempi_ignore_tkr -lmpi_mpifh -lquadmath -lm -lmpi -lpthread
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
