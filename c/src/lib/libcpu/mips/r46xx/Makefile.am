##
## $Id$
##

noinst_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am

if r46xx
noinst_PROGRAMS += vectorisrs.rel
vectorisrs_rel_SOURCES = vectorisrs/maxvectors.c vectorisrs/vectorisrs.c
vectorisrs_rel_CPPFLAGS = $(AM_CPPFLAGS)
vectorisrs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if r46xx
noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = ../timer/timer.c ../timer/gettime.S
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if r46xx
noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = ../clock/ckinit.c ../clock/clock.S ../clock/clock.h
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(top_srcdir)/../../../automake/local.am
