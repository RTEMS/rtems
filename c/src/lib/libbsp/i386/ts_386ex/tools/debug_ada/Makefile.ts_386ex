#
#  Makefile for debug example
#

MAIN=debug

# Tool paths
tooldir=/usr/local/rtems
rtemsdir=${tooldir}/ts_386ex

# Tool names
GCC=${tooldir}/bin/i386-rtemself-gcc
GNATMAKE=${tooldir}/bin/i386-rtemself-gnatmake
SIZE=${tooldir}/bin/i386-rtemself-size
SIS=${tooldir}/bin/sis
GDB=${tooldir}/bin/sis-gdb

CINCLUDES=

AINCLUDES=-i -I../ts_1325_ada

CARGS=-B${rtemsdir}/lib/ -specs bsp_specs -qrtems \
	-msoft-float -mno-fp-ret-in-387

all: init.o
	$(GNATMAKE) -O2 -g -gnata -gnatE -gnato -gnatn $(AINCLUDES) $(MAIN) \
	    -bargs -r \
	    -cargs $(CARGS) \
	    -largs $(CARGS) init.o
	$(SIZE) $(MAIN)

init.o: init.c
	$(GCC) -O2 -g  -Wall -ansi -fasm $(CARGS) $(CINCLUDES) -c init.c

run:
	$(SIS) $(MAIN)	

gdb:
	$(GDB) $(MAIN)	

clean:
	rm -f b_$(MAIN).c b_$(MAIN).o *.o *.ali $(MAIN)
