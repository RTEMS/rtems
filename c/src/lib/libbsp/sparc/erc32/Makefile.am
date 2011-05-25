##
## $Id: Makefile.am,v 1.54 2011/05/23 17:41:30 jennifer Exp 
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

SUBDIRS = . tools

include_HEADERS += include/erc32.h
include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = ../../sparc/shared/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds ../shared/startup/linkcmds.base

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../sparc/shared/bspgetworkarea.c \
    ../../sparc/shared/bsppretaskinghook.c ../../shared/bsppost.c \
    ../../shared/bspstart.c ../../shared/bootcard.c ../../shared/bspinit.c \
    ../../shared/sbrk.c startup/setvec.c startup/spurious.c \
    startup/erc32mec.c startup/boardinit.S startup/bspidle.c \
    startup/bspdelay.c
# ISR Handler
libbsp_a_SOURCES += ../../sparc/shared/irq_asm.S
# gnatsupp
libbsp_a_SOURCES += gnatsupp/gnatsupp.c ../../sparc/shared/gnatcommon.c
# console
libbsp_a_SOURCES += console/erc32_console.c ../../shared/console.c
# debugio
libbsp_a_SOURCES += console/debugputs.c
# clock
libbsp_a_SOURCES += clock/ckinit.c ../../../shared/clockdrv_shell.h
# timer
libbsp_a_SOURCES += timer/timer.c

if HAS_SMP
libbsp_a_SOURCES += ../../shared/smp/getcpuid.c ../../shared/smp/smp_stub.c \
    ../../shared/smp/bspsmp_wait_for.c
endif

if HAS_NETWORKING
erc32sonic_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += erc32sonic.rel
erc32sonic_rel_SOURCES = erc32sonic/erc32sonic.c
erc32sonic_rel_CPPFLAGS = $(AM_CPPFLAGS) $(erc32sonic_CPPFLAGS)
erc32sonic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD  = ../../../libcpu/@RTEMS_CPU@/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/reg_win.rel \
    ../../../libcpu/@RTEMS_CPU@/syscall.rel
if HAS_NETWORKING
libbsp_a_LIBADD += erc32sonic.rel
endif

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
