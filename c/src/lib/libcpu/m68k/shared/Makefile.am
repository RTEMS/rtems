##
## $Id$
##

CLEANDIRS =
EXTRA_DIST =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

## cache

cache_C_FILES = cache/cache.c
EXTRA_DIST += $(cache_C_FILES)
cache_OBJECTS = $(cache_C_FILES:cache/%.c=cache/$(ARCH)/%.$(OBJEXT))

common_C_FILES = ../../shared/src/cache_aligned_malloc.c
common_C_FILES += ../../shared/src/cache_manager.c
cache_OBJECTS += \
    $(common_C_FILES:../../shared/src/%.c=cache/$(ARCH)/%.$(OBJEXT))

cache/$(ARCH)/$(dirstamp):
	@$(mkdir_p) cache/$(ARCH)
	@: > cache/$(ARCH)/$(dirstamp)
cache/$(ARCH)/%.$(OBJEXT): cache/%.c cache/$(ARCH)/$(dirstamp)
	${COMPILE} -I$(srcdir)/cache $(M68K_CPPFLAGS) -o $@ -c $<
cache/$(ARCH)/%.$(OBJEXT): ../../shared/src/%.c cache/$(ARCH)/$(dirstamp)
	${COMPILE} -I$(srcdir)/cache $(M68K_CPPFLAGS) -o $@ -c $<
CLEANDIRS += cache/o-optimize cache/o-debug
noinst_DATA += $(cache_OBJECTS)

include_libcpudir = $(includedir)/libcpu

noinst_HEADERS = cache/cache_.h
include_libcpu_HEADERS = ../../shared/include/cache.h

## misc

misc_C_FILES = misc/memcpy.c misc/m68kidle.c
misc_OBJECTS = $(misc_C_FILES:misc/%.c=misc/$(ARCH)/%.$(OBJEXT))

# gcc doesn't recognize difference between the cpu32 and cpu32+ so we have to
if mcpu32p
M68K_CPPFLAGS = -D__mcpu32p__
endif
if mcf5272
M68K_CPPFLAGS = -Dmcf5272
endif

misc/$(ARCH)/$(dirstamp):
	@$(mkdir_p) misc/$(ARCH)
	@: > misc/$(ARCH)/$(dirstamp)
misc/$(ARCH)/%.$(OBJEXT): misc/%.c misc/$(ARCH)/$(dirstamp)
	${COMPILE} $(M68K_CPPFLAGS) -o $@ -c $<
CLEANDIRS += misc/o-optimize misc/o-debug
noinst_DATA += $(misc_OBJECTS)

## --
all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/cache.h: ../../shared/include/cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

include $(top_srcdir)/../../../automake/local.am
