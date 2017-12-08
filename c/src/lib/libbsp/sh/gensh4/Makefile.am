ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score
include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

include_sh_HEADERS = include/sh/sh4uart.h

include_rtems_score_HEADERS = include/rtems/score/sh7750_regs.h
include_rtems_score_HEADERS += include/rtems/score/ipl.h
include_rtems_score_HEADERS += include/rtems/score/iosh7750.h
include_rtems_score_HEADERS += include/rtems/score/ispsh7750.h
include_rtems_score_HEADERS += include/rtems/score/sh4_regs.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

include_HEADERS += include/sdram.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds.rom \
    startup/linkcmds.rom2ram

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += \
    ../../shared/bsppredriverhook.c ../shared/startup/bspstart.c \
    ../../shared/bspgetworkarea.c \
    ../../shared/bspclean.c ../../shared/sbrk.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c ../shared/bsphwinit.c
libbsp_a_SOURCES += ../../shared/bspreset.c
libbsp_a_SOURCES += startup/cpu_asm.c startup/ispsh7750.c
libbsp_a_SOURCES += clock/ckinit.c
libbsp_a_SOURCES += timer/timer.c
# console
libbsp_a_SOURCES += console/console.c ../../shared/dummy_printk_support.c
libbsp_a_SOURCES += console/sh4uart.c
# hw_init
libbsp_a_SOURCES += hw_init/hw_init.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../shared/include/cache_.h
libbsp_a_CPPFLAGS = -I$(srcdir)/../../shared/include

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
