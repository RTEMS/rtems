ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

_SUBDIRS = . tools



start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.flash

dist_project_lib_DATA += startup/gdbinit

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
    ../../shared/bspstart.c startup/init5206e.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/bspreset.c
# console
libbsp_a_SOURCES += console/console.c ../../shared/dummy_printk_support.c
# i2c
libbsp_a_SOURCES += i2c/i2c.c i2c/i2cdrv.c
# tod
libbsp_a_SOURCES += tod/ds1307.c tod/todcfg.c ../../shared/tod.c
# nvram
libbsp_a_SOURCES += nvram/nvram.c

libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/ckinit.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/mcfmbus.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/mcfuart.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/timer.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5206elite/dev/timerisr.S

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/m68k/mcf5206elite/headers.am
