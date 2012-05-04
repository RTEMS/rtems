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

include_HEADERS += ../../shared/tod.h
include_HEADERS += include/coverhd.h
include_HEADERS += include/page_table.h

nodist_include_HEADERS += $(top_srcdir)/../shared/mvme/mvme16x_hw.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = ../../m68k/shared/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bsppost.c \
    startup/bspstart.c ../../shared/bootcard.c  \
    ../../shared/bsppretaskinghook.c startup/page_table.c \
    ../../shared/bspgetworkarea.c startup/page_table.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
# clock
libbsp_a_SOURCES += clock/ckinit.c
# console
libbsp_a_SOURCES += console/console.c ../../shared/dummy_printk_support.c
# timer
libbsp_a_SOURCES += timer/timer.c timer/timerisr.S
# tod
libbsp_a_SOURCES += tod/tod.c

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = ../mvme167/network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif
if HAS_FPSP
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/fpsp.rel
endif

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
