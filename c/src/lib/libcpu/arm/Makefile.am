ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

EXTRA_DIST =

noinst_PROGRAMS =

include_bspdir = $(includedir)/bsp
include_libcpudir = $(includedir)/libcpu

include_bsp_HEADERS =
include_libcpu_HEADERS =

## shared/include
if shared

include_libcpu_HEADERS += shared/include/mmu.h
include_libcpu_HEADERS += shared/include/arm-cp15.h

## shared/arm920
noinst_PROGRAMS += shared/arm920.rel
shared_arm920_rel_SOURCES = shared/arm920/mmu.c
shared_arm920_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/shared/src
shared_arm920_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/local.am
