ACLOCAL_AMFLAGS = -I ../../../aclocal

noinst_PROGRAMS = nios2gen

nios2gen_SOURCES = nios2gen.c \
	bridges.c bridges.h \
	devices.c devices.h \
	clocks.c clocks.h \
	output.c output.h \
	linkcmds.c linkcmds.h \
	memory.c memory.h \
	ptf.h ptf.c

if HELP2MAN
man_MANS = nios2gen.1

nios2gen.1: nios2gen$(EXEEXT)
	$(HELP2MAN) -N ./nios2gen >$@
endif

include $(top_srcdir)/../../../automake/host.am
