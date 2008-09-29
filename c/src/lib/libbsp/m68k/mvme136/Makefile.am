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
noinst_PROGRAMS =

include_HEADERS += include/coverhd.h

EXTRA_DIST = ../../m68k/shared/start.S
start.$(OBJEXT): ../../m68k/shared/start.S
	$(CPPASCOMPILE) -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES = libbsp.a

startup_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bsppredriverhook.c  \
    startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/bsppretaskinghook.c ../../shared/bspgetworkarea.c \
    ../../shared/sbrk.c ../../m68k/shared/setvec.c
gnatsupp_SOURCES = ../../shared/gnatinstallhandler.c
clock_SOURCES = clock/ckinit.c
console_SOURCES = console/console.c
debugio_SOURCES = ../../shared/dummy_printk_support.c
timer_SOURCES = timer/timer.c timer/timerisr.S

if HAS_MP
shmsupp_SOURCES = shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c \
    shmsupp/mpisr.c
endif

libbsp_a_SOURCES = $(startup_SOURCES) $(gnatsupp_SOURCES) $(clock_SOURCES) \
    $(console_SOURCES) $(debugio_SOURCES) $(timer_SOURCES) $(shmsupp_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
