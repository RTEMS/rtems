##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = cpu.h cpuModel.h
include_libcpu_HEADERS += ../shared/include/cache.h

EXTRA_PROGRAMS += cache.rel
CLEANFILES += cache.rel
cache_rel_SOURCES = cache.c cache_.h ../shared/src/cache_aligned_malloc.c \
    ../shared/src/cache_manager.c ../shared/include/cache.h
cache_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += cache_g.rel
CLEANFILES += cache_g.rel
cache_g_rel_SOURCES = $(cache_rel_SOURCES)
cache_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
cache_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += cache$(LIB_VARIANT).rel

EXTRA_PROGRAMS += score.rel
CLEANFILES += score.rel
score_rel_SOURCES = displayCpu.c cpuModel.S cpuModel.h idtr.S cpu.h
score_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
score_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += score_g.rel
CLEANFILES += score_g.rel
score_g_rel_SOURCES = $(score_rel_SOURCES)
score_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
score_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += score$(LIB_VARIANT).rel

EXTRA_PROGRAMS += page.rel
CLEANFILES += page.rel
page_rel_SOURCES = page.c cpu.h
page_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
page_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += page_g.rel
CLEANFILES += page_g.rel
page_g_rel_SOURCES = $(page_rel_SOURCES)
page_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
page_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += page$(LIB_VARIANT).rel

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/cpu.h: cpu.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cpu.h

$(PROJECT_INCLUDE)/libcpu/cpuModel.h: cpuModel.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cpuModel.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cpuModel.h

$(PROJECT_INCLUDE)/libcpu/cache.h: ../shared/include/cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
