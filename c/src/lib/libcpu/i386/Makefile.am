ACLOCAL_AMFLAGS = -I ../../../aclocal

noinst_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = cpu.h cpuModel.h
include_libcpu_HEADERS += ../shared/include/cache.h
include_libcpu_HEADERS += byteorder.h

noinst_PROGRAMS += cache.rel
cache_rel_SOURCES = cache.c cache_.h ../shared/src/cache_aligned_malloc.c \
    ../shared/src/cache_manager.c ../shared/include/cache.h
cache_rel_CPPFLAGS = $(AM_CPPFLAGS)
cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += score.rel
score_rel_SOURCES = displayCpu.c cpuModel.S cpuModel.h idtr.S cpu.h
score_rel_CPPFLAGS = $(AM_CPPFLAGS)
score_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += page.rel
page_rel_SOURCES = page.c cpu.h
page_rel_CPPFLAGS = $(AM_CPPFLAGS)
page_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/local.am
