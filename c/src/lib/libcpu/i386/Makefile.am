##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

noinst_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = cpu.h cpuModel.h
include_libcpu_HEADERS += ../shared/include/cache.h

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

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
