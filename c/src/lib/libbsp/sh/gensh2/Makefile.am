ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score
include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

include_sh_HEADERS = include/sh/io_types.h
include_sh_HEADERS += include/sh/sci.h
include_sh_HEADERS += include/sh/sh7_pfc.h
include_sh_HEADERS += include/sh/sh7_sci.h
include_sh_HEADERS += include/sh/sci_termios.h

include_rtems_score_HEADERS = include/rtems/score/ispsh7045.h
include_rtems_score_HEADERS += include/rtems/score/iosh7045.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h


EXTRA_DIST += start/start.ram start/start.rom
noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds.ram \
    startup/linkcmds.rom

noinst_LIBRARIES += libbsp.a
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
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../shared/include/cache_.h
libbsp_a_CPPFLAGS = -I$(srcdir)/../../shared/include

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
