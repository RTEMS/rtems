ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

include_HEADERS += include/sdram.h
include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds.rom \
    startup/linkcmds.rom2ram

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/bsppredriverhook.c ../shared/bspstart.c \
    ../../shared/bsppretaskinghook.c ../../shared/bspgetworkarea.c \
    ../../shared/bspclean.c ../../shared/sbrk.c ../../shared/bootcard.c \
    ../../shared/gnatinstallhandler.c ../shared/bsphwinit.c
# console
libbsp_a_SOURCES += console/console.c ../../shared/dummy_printk_support.c
# hw_init
libbsp_a_SOURCES += hw_init/hw_init.c

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/sci.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/score.rel

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
