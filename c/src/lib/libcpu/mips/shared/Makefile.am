##
## $Id$
##

EXTRA_DIST =
CLEANDIRS =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

## cache
include_libcpudir = $(includedir)/libcpu

noinst_HEADERS = cache/cache_.h
include_libcpu_HEADERS = ../../shared/include/cache.h

cache_C_FILES = cache/cache.c
common_C_FILES = ../../shared/src/cache_aligned_malloc.c
common_C_FILES += ../../shared/src/cache_manager.c

cache_rel_OBJECTS = $(cache_C_FILES:cache/%.c=cache/$(ARCH)/%.$(OBJEXT))
cache_rel_OBJECTS += \
    $(common_C_FILES:../../shared/src/%.c=cache/$(ARCH)/%.$(OBJEXT))

AM_CPPFLAGS += -I$(srcdir)/cache

cache/$(ARCH)/$(dirstamp):
	@$(mkdir_p) cache/$(ARCH)
	@: > cache/$(ARCH)/$(dirstamp)
cache/$(ARCH)/%.$(OBJEXT): cache/%.c cache/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
cache/$(ARCH)/%.$(OBJEXT): ../../shared/src/%.c cache/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
cache/$(ARCH)/cache.rel: $(cache_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += cache/o-optimize cache/o-debug
noinst_DATA += cache/$(ARCH)/cache.rel

## interrupts

interrupts_C_FILES = interrupts/installisrentries.c \
    interrupts/vectorexceptions.c
interrupts_S_FILES = interrupts/isr_entries.S

interrupts_rel_OBJECTS = \
    $(interrupts_C_FILES:interrupts/%.c=interrupts/$(ARCH)/%.$(OBJEXT)) \
    $(interrupts_S_FILES:interrupts/%.S=interrupts/$(ARCH)/%.$(OBJEXT))

if tx39
EXTRA_FLAGS = -DTX39
endif

if mongoosev
EXTRA_FLAGS = -DMONGOOSEV
endif

AM_CPPFLAGS += $(EXTRA_FLAGS)

interrupts/$(ARCH)/$(dirstamp):
	@$(mkdir_p) interrupts/$(ARCH)
	@: > interrupts/$(ARCH)/$(dirstamp)
interrupts/$(ARCH)/%.$(OBJEXT): interrupts/%.c interrupts/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
interrupts/$(ARCH)/%.$(OBJEXT): interrupts/%.S interrupts/$(ARCH)/$(dirstamp)
	${CCASCOMPILE} -o $@ -c $<
interrupts/$(ARCH)/interrupts.rel: $(interrupts_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += interrupts/o-optimize interrupts/o-debug
noinst_DATA += interrupts/$(ARCH)/interrupts.rel

## --

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/cache.h: ../../shared/include/cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
