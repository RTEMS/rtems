ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

include_HEADERS += include/coverhd.h

## Zilog component header files
include_rtems_zilogdir = $(includedir)/rtems/zilog
include_rtems_zilog_HEADERS = ../../shared/include/zilog/z8530.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = ../../m68k/shared/start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/setvec.c \
    ../../shared/bspgetworkarea.c ../../shared/setvec.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c
# clock
libbsp_a_SOURCES += ../mvme147/clock/ckinit.c
# console
libbsp_a_SOURCES += ../mvme147/console/console.c \
    ../../shared/dummy_printk_support.c
# timer
libbsp_a_SOURCES += ../mvme147/timer/timer.c ../mvme147/timer/timerisr.S

if HAS_MP
# shmsupp
libbsp_a_SOURCES += shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c \
    shmsupp/mpisr.c
endif

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
