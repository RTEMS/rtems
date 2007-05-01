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

EXTRA_DIST = ../../m68k/shared/start.S
start.$(OBJEXT): ../../m68k/shared/start.S
	$(CPPASCOMPILE) -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../m68k/shared/m68kpretaskinghook.c \
    ../../shared/sbrk.c ../../m68k/shared/setvec.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += gnatsupp.rel
gnatsupp_rel_SOURCES = ../../shared/gnatinstallhandler.c
gnatsupp_rel_CPPFLAGS = $(AM_CPPFLAGS)
gnatsupp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += debugio.rel
debugio_rel_SOURCES = ../../shared/dummy_printk_support.c
debugio_rel_CPPFLAGS = $(AM_CPPFLAGS)
debugio_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c timer/timerisr.S
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_MP
noinst_PROGRAMS += shmsupp.rel
shmsupp_rel_SOURCES = shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c \
    shmsupp/mpisr.c
shmsupp_rel_CPPFLAGS = $(AM_CPPFLAGS)
shmsupp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = gnatsupp.rel clock.rel console.rel debugio.rel timer.rel
if HAS_MP
libbsp_a_LIBADD += shmsupp.rel
endif
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

libbsp_a_LIBADD += \
startup_rel-bspclean.o \
startup_rel-bsplibc.o \
startup_rel-bsppost.o \
startup_rel-bspstart.o \
startup_rel-bootcard.o \
startup_rel-m68kpretaskinghook.o \
startup_rel-sbrk.o \
startup_rel-setvec.o

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
