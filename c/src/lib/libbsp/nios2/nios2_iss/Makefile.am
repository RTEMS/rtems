##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am


dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += include/coverhd.h

EXTRA_DIST = start/start.S start/crtnn.s times
crtnn.$(OBJEXT): start/crtnn.s
	$(CPPASCOMPILE) -o $@ -c $<
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT) crtnn.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bspreset_loop.c ../../shared/bsppost.c startup/bspstart.c \
    ../../shared/bsppretaskinghook.c ../../shared/bspgetworkarea.c \
    ../../shared/bootcard.c ../../shared/sbrk.c startup/setvec.c \
    ../../shared/gnatinstallhandler.c
clock_SOURCES = clock/clock.c
console_SOURCES = console/console.c
timer_SOURCES = timer/timer.c
shmsupp_SOURCES = shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c \
    shmsupp/mpisr.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES) $(shmsupp_SOURCES)

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
