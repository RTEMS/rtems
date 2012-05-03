ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

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

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/gdbinit \
    startup/linkcmds.flash

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
    ../../shared/bsppost.c ../../shared/bsppretaskinghook.c \
    ../../shared/bspstart.c startup/init5206e.c ../../shared/bootcard.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
# console
libbsp_a_SOURCES += console/console.c ../../shared/dummy_printk_support.c
# i2c
libbsp_a_SOURCES += i2c/i2c.c i2c/i2cdrv.c
# tod
libbsp_a_SOURCES += tod/ds1307.c tod/todcfg.c ../../shared/tod.c
# nvram
libbsp_a_SOURCES += nvram/nvram.c

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/mcfuart.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5206/mbus.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
