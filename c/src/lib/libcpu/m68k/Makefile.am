##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

SUBDIRS = m68040 mcf5206 mcf5272 mcf5282

CLEANFILES =
DISTCLEANFILES =
noinst_PROGRAMS =

if shared
include_libcpudir = $(includedir)/libcpu

## shared/cache
include_libcpu_HEADERS = ../shared/include/cache.h

noinst_PROGRAMS += shared/cache.rel
shared_cache_rel_SOURCES = shared/cache/cache.c shared/cache/cache_.h \
    ../shared/src/cache_aligned_malloc.c \
    ../shared/src/cache_manager.c
shared_cache_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/shared/cache
shared_cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## shared/misc

# gcc doesn't recognize difference between the cpu32 and cpu32+ so we have to
if mcpu32p
M68K_CPPFLAGS = -D__mcpu32p__
endif
if mcf5272
M68K_CPPFLAGS = -Dmcf5272
endif

noinst_PROGRAMS += shared/misc.rel
shared_misc_rel_SOURCES = shared/misc/memcpy.c shared/misc/m68kidle.c shared/misc/memProbe.c
shared_misc_rel_CPPFLAGS = $(AM_CPPFLAGS) $(M68K_CPPFLAGS)
shared_misc_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(srcdir)/preinstall.am

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
