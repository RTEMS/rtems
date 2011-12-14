##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h
include_bsp_HEADERS = ../shared/liblnk/regs.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S ../shared/liblnk/regs.h
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bsplibc.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/bsppost.c
libbsp_a_SOURCES += ../../shared/bsppretaskinghook.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/setvec.c
libbsp_a_SOURCES += ../../shared/bspreset.c
# clock
libbsp_a_SOURCES += clock/clockdrv.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
# console
libbsp_a_SOURCES += ../../shared/console-polled.c
libbsp_a_SOURCES += console/console-io.c
# timer
libbsp_a_SOURCES += timer/timer.c

libbsp_a_LIBADD  = ../../../libcpu/@RTEMS_CPU@/shared/cache.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/interrupts.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/tx39/vectorisrs.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cache.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
