CC = mips64orion-idt-elf-gcc
CFLAGS = -g -Wa,-ahld -Wall -membedded-data -O3 
AS = mips64orion-idt-elf-as
ASFLAGS = -ahld
LD = mips64orion-idt-elf-ld
LDFLAGS = -t -s

# Inference rules

.SUFFIXES: $(SUFFIXES) .out .ld

.ld.out:
	$(LD) $(LDFLAGS) -T $< -Map $*.map -o $*.out
.c.o:
	$(CC) $(CFLAGS) -c $< >$*.L

.S.o:
	$(CC) $(CFLAGS) -c $< >$*.L

.s.o:
	$(AS) $(ASFLAGS) -o $*.o $< >$*.L

# Targets

r46kstub.hex: r46kstub.out
	mips64orion-idt-elf-objcopy -S -R .bss -R .data -R .reginfo \
					-O srec r46kstub.out r46kstub.hex

clean:
	rm -f *.L *.map *.o *.out *.hex

# Dependencies

r46kstub.out: r46kstub.ld r46kstub.o stubinit.o

r46kstub.o: mips_opcode.h r4600.h limits.h gdb_if.h r46kstub.c

stubinit.o: r4600.h ioaddr.h gdb_if.h stubinit.S
