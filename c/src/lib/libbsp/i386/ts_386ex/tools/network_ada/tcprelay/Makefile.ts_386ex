#
#  Makefile for tcprelay example
#

MAIN=tcprelay

# Tool paths
tooldir=/usr/local/rtems
rtemsdir=${tooldir}/ts_386ex

# Tool names
GCC=${tooldir}/bin/i386-rtemself-gcc
GNATMAKE=${tooldir}/bin/i386-rtemself-gnatmake
SIZE=${tooldir}/bin/i386-rtemself-size
SIS=${tooldir}/bin/sis
GDB=${tooldir}/bin/sis-gdb

CINCLUDES=-I/usr/local/rtems/ts_386ex/lib/include/networking

AINCLUDES=-i -I/usr/local/rtems/lib/adasockets

CARGS=-B${rtemsdir}/lib/ -specs bsp_specs -qrtems

all: init.o print_error.o
	$(GNATMAKE) -O -gnata -gnatE -gnato $(AINCLUDES) $(MAIN) -g \
	    -bargs -r \
	    -cargs $(CARGS) \
	    -largs $(CARGS) init.o print_error.o
	$(SIZE) $(MAIN)



init.o: init.c
	$(GCC) -O4 -g  -Wall -ansi -fasm $(CARGS) $(CINCLUDES) -c init.c

print_error.o: print_error.c
	$(GCC) -O4 -g  -Wall -ansi -fasm $(CARGS) $(CINCLUDES) -c print_error.c

run:
	$(SIS) $(MAIN)	

gdb:
	$(GDB) $(MAIN)	

clean:
	rm -f b_$(MAIN).c b_$(MAIN).o *.o *.ali $(MAIN)
