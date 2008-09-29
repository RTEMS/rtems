##
## $Id$
##

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

nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S start/regs.h start/mg5.h
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsppretaskinghook.c \
    ../../shared/bsppredriverhook.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c \
    ../../shared/setvec.c ../../shared/bspgetworkarea.c
# clock
libbsp_a_SOURCES += clock/clockdrv.c
# console
libbsp_a_SOURCES += console/conscfg.c ../../shared/console.c
# timer
libbsp_a_SOURCES += timer/timer.c

gdbstub_CPPFLAGS = -I$(srcdir)/../../mips/shared/gdbstub
noinst_PROGRAMS += gdbstub.rel
gdbstub_rel_SOURCES = ../../mips/shared/gdbstub/mips-stub.c \
    startup/gdb-support.c ../../shared/gdbstub/rtems-stub-glue.c
gdbstub_rel_CPPFLAGS = $(AM_CPPFLAGS) $(gdbstub_CPPFLAGS)
gdbstub_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/interrupts.rel \
    ../../../libcpu/@RTEMS_CPU@/mongoosev/duart.rel \
    ../../../libcpu/@RTEMS_CPU@/mongoosev/vectorisrs.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
