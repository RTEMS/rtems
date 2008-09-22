##
## $Id$
##

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = shared/include/utility.h

noinst_HEADERS = shared/include/cache.h \
	shared/src/cache_aligned_malloc.c \
	shared/src/cache_manager.c

SUBDIRS = @libcpu_cpu_subdir@
DIST_SUBDIRS = @libcpu_cpu_subdir@

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
