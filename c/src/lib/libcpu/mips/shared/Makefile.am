##
## $Id$
##

EXTRA_DIST =
CLEANFILES =
noinst_DATA =
EXTRA_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

## cache
include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = ../../shared/include/cache.h

EXTRA_PROGRAMS += cache.rel
CLEANFILES += cache.rel
cache_rel_SOURCES = cache/cache.c ../../shared/src/cache_aligned_malloc.c \
    ../../shared/src/cache_manager.c cache/cache_.h
cache_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) -I$(srcdir)/cache
cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += cache_g.rel
CLEANFILES += cache_g.rel
cache_g_rel_SOURCES = $(cache_rel_SOURCES)
cache_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) -I$(srcdir)/cache
cache_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += cache$(LIB_VARIANT).rel

## interrupts

if tx39
interrupts_CPPFLAGS = -DTX39
endif

if mongoosev
interrupts_CPPFLAGS = -DMONGOOSEV
endif

EXTRA_PROGRAMS += interrupts.rel
CLEANFILES += interrupts.rel
interrupts_rel_SOURCES = interrupts/installisrentries.c \
    interrupts/vectorexceptions.c interrupts/isr_entries.S
interrupts_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) \
    $(interrupts_CPPFLAGS)
interrupts_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += interrupts_g.rel
CLEANFILES += interrupts_g.rel
interrupts_g_rel_SOURCES = $(interrupts_rel_SOURCES)
interrupts_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) \
    $(interrupts_CPPFLAGS)
interrupts_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += interrupts$(LIB_VARIANT).rel

all-local: $(PREINSTALL_FILES)

## --

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/cache.h: ../../shared/include/cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

CLEANFILES += $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
