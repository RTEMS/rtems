##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

DISTCLEANFILES =
CLEANFILES =
EXTRA_PROGRAMS =
noinst_DATA =

include_libcpudir = $(includedir)/libcpu
include_libcpu_HEADERS = $(top_srcdir)/../shared/include/cache.h

EXTRA_PROGRAMS += cache.rel
CLEANFILES += cache.rel
cache_rel_SOURCES = cache/cache.c cache/cache_.h \
    ../shared/src/cache_aligned_malloc.c ../shared/src/cache_manager.c
cache_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V) -I$(srcdir)/cache
cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += cache_g.rel
CLEANFILES += cache_g.rel
cache_g_rel_SOURCES = $(cache_rel_SOURCES)
cache_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V) -I$(srcdir)/cache
cache_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if has_instruction_cache
cache_rel_CPPFLAGS += -DHAS_INSTRUCTION_CACHE
cache_g_rel_CPPFLAGS += -DHAS_INSTRUCTION_CACHE
endif

noinst_DATA += cache$(LIB_VARIANT).rel

EXTRA_PROGRAMS += syscall.rel
CLEANFILES += syscall.rel
syscall_rel_SOURCES = syscall/syscall.S syscall/syscall.h
syscall_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
syscall_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += syscall_g.rel
CLEANFILES += syscall_g.rel
syscall_g_rel_SOURCES = $(syscall_rel_SOURCES)
syscall_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
syscall_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += syscall$(LIB_VARIANT).rel

EXTRA_PROGRAMS += reg_win.rel
CLEANFILES += reg_win.rel
reg_win_rel_SOURCES = reg_win/window.S
reg_win_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
reg_win_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += reg_win_g.rel
CLEANFILES += reg_win_g.rel
reg_win_g_rel_SOURCES = $(reg_win_rel_SOURCES)
reg_win_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
reg_win_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += reg_win$(LIB_VARIANT).rel

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/cache.h: $(top_srcdir)/../shared/include/cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES += $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
