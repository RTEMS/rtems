##
## $Id$
##

noinst_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am

if shared
include_libcpudir = $(includedir)/libcpu

## cache
include_libcpu_HEADERS = ../../shared/include/cache.h

noinst_PROGRAMS += cache.rel
cache_rel_SOURCES = cache/cache.c cache/cache_.h \
    ../../shared/src/cache_aligned_malloc.c \
    ../../shared/src/cache_manager.c
cache_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/cache
cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## misc

# gcc doesn't recognize difference between the cpu32 and cpu32+ so we have to
if mcpu32p
M68K_CPPFLAGS = -D__mcpu32p__
endif
if mcf5272
M68K_CPPFLAGS = -Dmcf5272
endif

noinst_PROGRAMS += misc.rel
misc_rel_SOURCES = misc/memcpy.c misc/m68kidle.c misc/memProbe.c
misc_rel_CPPFLAGS = $(AM_CPPFLAGS) $(M68K_CPPFLAGS)
misc_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

## --
all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if shared
$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/cache.h: ../../shared/include/cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
