#
#  Makefile for TS-1325 Utilities example
#

MAIN=ts1325_test

# Tool paths
tooldir=/usr/local/rtems
rtemsdir=${tooldir}/rtems/ts_386ex

# Tool names
GCC=${tooldir}/bin/i386-rtems-gcc
GNATMAKE=${tooldir}/bin/i386-rtems-gnatmake
SIZE=${tooldir}/bin/i386-rtems-size
SIS=${tooldir}/bin/sis
GDB=${tooldir}/bin/sis-gdb

CARGS=-B${rtemsdir}/lib/ -specs bsp_specs -qrtems \
-msoft-float -mno-fp-ret-in-387
#CARGS=-B/usr/local/rtems/tools/build-i386-rtems/ts_386ex/lib/ -specs bsp_specs -qrtems

all: init.o 
	$(GNATMAKE)  -O -gnata -gnatE -gnato $(MAIN) -g \
	    -bargs -r \
	    -cargs $(CARGS) \
	    -largs $(CARGS) init.o
	$(SIZE) $(MAIN)



init.o: init.c
	$(GCC) -O4 -g  -Wall -ansi -fasm $(CARGS) -c init.c

run:
	$(SIS) $(MAIN)	

gdb:
	$(GDB) $(MAIN)	

clean:
	rm -f b_$(MAIN).c b_$(MAIN).o *.o *.ali $(MAIN)
