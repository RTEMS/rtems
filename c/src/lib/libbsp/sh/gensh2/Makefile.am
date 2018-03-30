ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.ram
dist_project_lib_DATA += startup/linkcmds.rom

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

SHGEN = $(PROJECT_TOPdir)/tools/cpu/sh/shgen

scitab.c: $(SHGEN)
	$(SHGEN) -H @CPU_CLOCK_RATE_HZ@ sci > $@
BUILT_SOURCES = scitab.c
CLEANFILES = scitab.c

# startup
libbsp_a_SOURCES += startup/hw_init.c \
    ../../shared/bsppredriverhook.c \
    ../shared/startup/bspstart.c \
    ../../shared/bspgetworkarea.c ../../shared/sbrk.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c ../../shared/bspclean.c \
    ../shared/bsphwinit.c
libbsp_a_SOURCES += ../../shared/bspreset.c
libbsp_a_SOURCES += clock/ckinit.c
libbsp_a_SOURCES += startup/cpu_asm.c startup/ispsh7045.c
libbsp_a_SOURCES += timer/timer.c
# scitab
libbsp_a_SOURCES += scitab.c
# console
libbsp_a_SOURCES += ../../sh/shared/console.c console/config.c
libbsp_a_SOURCES += console/sci.c
libbsp_a_SOURCES += console/sci_termios.c
# debugio
libbsp_a_SOURCES += ../../shared/dummy_printk_support.c

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/sh/gensh2/headers.am
