##
## $Id$
##

noinst_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am

## cache
include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = ../../shared/include/cache.h

noinst_PROGRAMS += cache.rel
cache_rel_SOURCES = cache/cache.c ../../shared/src/cache_aligned_malloc.c \
    ../../shared/src/cache_manager.c cache/cache_.h
cache_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) -I$(srcdir)/cache
cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## interrupts

if tx39
interrupts_CPPFLAGS = -DTX39
endif

if mongoosev
interrupts_CPPFLAGS = -DMONGOOSEV
endif

noinst_PROGRAMS += interrupts.rel
interrupts_rel_SOURCES = interrupts/installisrentries.c \
    interrupts/vectorexceptions.c interrupts/isr_entries.S
interrupts_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) \
    $(interrupts_CPPFLAGS)
interrupts_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

    $(interrupts_CPPFLAGS)

all-local: $(PREINSTALL_FILES)

## --

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/cache.h: ../../shared/include/cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
