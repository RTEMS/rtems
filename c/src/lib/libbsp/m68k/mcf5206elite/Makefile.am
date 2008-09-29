##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

SUBDIRS = . tools

include_HEADERS += include/coverhd.h
include_HEADERS += include/i2c.h
include_HEADERS += include/i2cdrv.h
include_HEADERS += include/ds1307.h
include_HEADERS += include/nvram.h
nodist_include_HEADERS += ../../shared/tod.h

EXTRA_DIST = times

EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/gdbinit \
    startup/linkcmds.flash

startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
    ../../shared/bsppost.c ../../shared/bsppretaskinghook.c \
    ../../shared/bspstart.c startup/init5206e.c ../../shared/bootcard.c \
    ../../shared/sbrk.c ../../m68k/shared/setvec.c \
    ../../shared/gnatinstallhandler.c
console_SOURCES = console/console.c ../../shared/dummy_printk_support.c
i2c_SOURCES = i2c/i2c.c i2c/i2cdrv.c
tod_SOURCES = tod/ds1307.c tod/todcfg.c ../../shared/tod.c
nvram_SOURCES = nvram/nvram.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(console_SOURCES) \
    $(i2c_SOURCES) $(tod_SOURCES) $(nvram_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/mcfuart.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/mbus.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
