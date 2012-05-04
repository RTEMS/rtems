ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

CLEANFILES =
DISTCLEANFILES =
noinst_PROGRAMS =

if shared
include_libcpudir = $(includedir)/libcpu

## shared/cache
include_libcpu_HEADERS = ../shared/include/cache.h
noinst_PROGRAMS += shared/cache.rel
shared_cache_rel_SOURCES = ../shared/src/no_cache.c shared/cache/cache_.h \
    ../shared/src/cache_aligned_malloc.c \
    ../shared/src/cache_manager.c
shared_cache_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/shared/cache
shared_cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## shared/misc
noinst_PROGRAMS += shared/misc.rel
shared_misc_rel_SOURCES = shared/misc/memcpy.c
shared_misc_rel_CPPFLAGS = $(AM_CPPFLAGS) $(LM32_CPPFLAGS)
shared_misc_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(srcdir)/preinstall.am

include $(top_srcdir)/../../../automake/local.am
