ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

CLEANFILES =
DISTCLEANFILES =
noinst_PROGRAMS =

include_libcpudir = $(includedir)/libcpu

## shared/cache
include_libcpu_HEADERS = ../shared/include/cache.h
include_libcpu_HEADERS += shared/cache/cache_.h
noinst_PROGRAMS += shared/cache.rel
shared_cache_rel_SOURCES = shared/cache/cache.c ../shared/src/cache_manager.c
shared_cache_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/shared/cache
shared_cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include $(srcdir)/preinstall.am

include $(top_srcdir)/../../../automake/local.am
