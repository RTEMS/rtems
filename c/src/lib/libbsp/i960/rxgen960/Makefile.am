##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

nodist_include_HEADERS += ../../shared/include/coverhd.h

# FIXME: this file should be called start.S
EXTRA_DIST = start/rxgen_romld.S
rxgen_romld$(LIB_VARIANT).$(OBJEXT): start/rxgen_romld.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = rxgen_romld$(LIB_VARIANT).$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds.real \
startup/rom.ld

EXTRA_PROGRAMS += clock.rel
CLEANFILES += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += clock_g.rel
CLEANFILES += clock_g.rel
clock_g_rel_SOURCES = $(clock_rel_SOURCES)
clock_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
clock_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += clock$(LIB_VARIANT).rel

EXTRA_PROGRAMS += console.rel
CLEANFILES += console.rel
console_rel_SOURCES = console/concntl.h console/console.c console/serial.c \
    console/serial.h
console_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += console_g.rel
CLEANFILES += console_g.rel
console_g_rel_SOURCES = $(console_rel_SOURCES)
console_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
console_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += console$(LIB_VARIANT).rel

EXTRA_PROGRAMS += startup.rel
CLEANFILES += startup.rel
startup_rel_SOURCES = include/rxgen960_config.h startup/asmfault.h \
    startup/asmfault.S startup/asmstub.h startup/asmstub.S \
    ../../shared/bootcard.c ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c startup/cntrltbl.c \
    startup/cntrltbl.h startup/exit.c startup/fault.c startup/fault.h \
    startup/faultret.h startup/flttbl.c startup/flttbl.h startup/frmstr.c \
    startup/frmstr.h startup/i960.h startup/ihandler.h startup/ihandler.S \
    startup/intrtbl.c startup/intrtbl.h startup/kkprintf.c \
    ../../shared/main.c startup/main.h startup/memchnl.h startup/nmi.c \
    startup/nulsystbl.c startup/pmc901_memmap.h startup/prcb.c \
    startup/prcb.h startup/rom_cntrltbl.c startup/rom_ibr.c \
    startup/rom_ibr.h startup/rommon.h startup/rom_prcb.c \
    ../../shared/sbrk.c startup/sctns.c startup/sctns.h startup/setvec.c \
    startup/systbl.c startup/systbl.h startup/time.h
startup_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += startup_g.rel
CLEANFILES += startup_g.rel
startup_g_rel_SOURCES = $(startup_rel_SOURCES)
startup_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
startup_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += startup$(LIB_VARIANT).rel

EXTRA_PROGRAMS += timer.rel
CLEANFILES += timer.rel
timer_rel_SOURCES = timer/timer.c
UNUSED_FILES = timer/timerisr.S
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += timer_g.rel
CLEANFILES += timer_g.rel
timer_g_rel_SOURCES = $(timer_rel_SOURCES)
timer_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
timer_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += timer$(LIB_VARIANT).rel

EXTRA_LIBRARIES = libbsp.a
CLEANFILES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup$(LIB_VARIANT).rel clock$(LIB_VARIANT).rel \
    console$(LIB_VARIANT).rel timer$(LIB_VARIANT).rel
libbsp_a_LIBADD += \
    ../../../libcpu/@RTEMS_CPU@/i960rp/score$(LIB_VARIANT).rel

EXTRA_LIBRARIES += libbsp_g.a
CLEANFILES += libbsp_g.a
libbsp_g_a_SOURCES = $(libbsp_a_SOURCES)
libbsp_g_a_LIBADD = $(libbsp_a_LIBADD)

noinst_DATA += libbsp$(LIB_VARIANT).a

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

PREINSTALL_DIRS =
TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/coverhd.h: ../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/rxgen_romld$(LIB_VARIANT).$(OBJEXT): rxgen_romld$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/rxgen_romld$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/rxgen_romld$(LIB_VARIANT).$(OBJEXT)

$(PROJECT_LIB)/linkcmds: startup/linkcmds $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/linkcmds
TMPINSTALL_FILES += $(PROJECT_LIB)/linkcmds

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
