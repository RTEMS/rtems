##
## $Id$
##

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am

# include
if shared
include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = include/io.h include/mmu.h include/page.h \
    include/byteorder.h include/pgtable.h

EXTRA_PROGRAMS += cpuIdent.rel
CLEANFILES += cpuIdent.rel
cpuIdent_rel_SOURCES = include/cpuIdent.c include/cpuIdent.h
cpuIdent_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
cpuIdent_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += cpuIdent_g.rel
CLEANFILES += cpuIdent_g.rel
cpuIdent_g_rel_SOURCES = $(cpuIdent_rel_SOURCES)
cpuIdent_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
cpuIdent_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += cpuIdent$(LIB_VARIANT).rel
include_libcpu_HEADERS += include/cpuIdent.h

## cache
EXTRA_PROGRAMS += cache.rel
CLEANFILES += cache.rel
cache_rel_SOURCES = src/cache.c src/cache_.h \
    ../../shared/src/cache_aligned_malloc.c ../../shared/src/cache_manager.c \
    ../../shared/include/cache.h
cache_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) -I$(srcdir)/src
cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += cache_g.rel
CLEANFILES += cache_g.rel
cache_g_rel_SOURCES = $(cache_rel_SOURCES)
cache_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) -I$(srcdir)/src
cache_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += cache$(LIB_VARIANT).rel
include_libcpu_HEADERS += ../../shared/include/cache.h

## stack
EXTRA_PROGRAMS += stack.rel
CLEANFILES += stack.rel
stack_rel_SOURCES = src/stack.c include/spr.h src/stackTrace.h
stack_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
stack_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += stack_g.rel
CLEANFILES += stack_g.rel
stack_g_rel_SOURCES = $(stack_rel_SOURCES)
stack_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
stack_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += stack$(LIB_VARIANT).rel
include_libcpu_HEADERS += include/spr.h
include_libcpu_HEADERS += src/stackTrace.h
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if shared
$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/io.h: include/io.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/io.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/io.h

$(PROJECT_INCLUDE)/libcpu/mmu.h: include/mmu.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/mmu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/mmu.h

$(PROJECT_INCLUDE)/libcpu/page.h: include/page.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/page.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/page.h

$(PROJECT_INCLUDE)/libcpu/byteorder.h: include/byteorder.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/byteorder.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/byteorder.h

$(PROJECT_INCLUDE)/libcpu/pgtable.h: include/pgtable.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/pgtable.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/pgtable.h

$(PROJECT_INCLUDE)/libcpu/cpuIdent.h: include/cpuIdent.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cpuIdent.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cpuIdent.h

$(PROJECT_INCLUDE)/libcpu/cache.h: ../../shared/include/cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

$(PROJECT_INCLUDE)/libcpu/spr.h: include/spr.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/spr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/spr.h

$(PROJECT_INCLUDE)/libcpu/stackTrace.h: src/stackTrace.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/stackTrace.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/stackTrace.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
