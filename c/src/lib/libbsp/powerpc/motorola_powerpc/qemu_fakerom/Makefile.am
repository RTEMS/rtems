## NOTE: This Makefile.am does not apply RTEMS's compilation rules.
ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

AUTOMAKE_OPTIONS = no-exeext

project_lib_DATA  = qemu_fakerom.$(OBJEXT)
project_lib_DATA += qemu_fakerom.elf
project_lib_DATA += qemu_fakerom.bin


## IMPORTANT: head.S must be first, T. Straumann 12/17/2001
qemu_fakerom.$(OBJEXT): ../../../powerpc/shared/bootloader/qemu_fakerom.S
	$(CPPASCOMPILE) -c -o $@ $^

qemu_fakeres.$(OBJEXT): ../../../powerpc/shared/bootloader/qemu_fakeres.c
	$(CC) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -c -o $@ $^

qemu_fakerom.elf: qemu_fakerom.$(OBJEXT) qemu_fakeres.$(OBJEXT)
	$(LD) -o $@ $^ -nostdlib -Ttext 0xfff00000 --section-start=.romentry=0xfffffffc

qemu_fakerom.bin: qemu_fakerom.$(OBJEXT) qemu_fakeres.$(OBJEXT)
	$(LD) -o $@ $^ --oformat binary -nostdlib -Ttext 0xfff00000 --section-start=.romentry=0xfffffffc

include $(top_srcdir)/../../../../automake/local.am
