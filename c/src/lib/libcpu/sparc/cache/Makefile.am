##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

src_C_FILES = cache.c
common_C_FILES = ../../shared/src/cache_aligned_malloc.c
common_C_FILES += ../../shared/src/cache_manager.c

OBJS = $(src_C_FILES:%.c=$(ARCH)/%.$(OBJEXT))
OBJS += $(common_C_FILES:../../shared/src/%.c=$(ARCH)/%.$(OBJEXT))

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = ../../shared/include/cache.h
noinst_HEADERS = cache_.h

if has_instruction_cache
CACHE_DEFINE = -DHAS_INSTRUCTION_CACHE
endif

AM_CPPFLAGS += -I$(srcdir) $(CACHE_DEFINE)

$(ARCH)/%.$(OBJEXT): ../../shared/src/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

all-local: $(PREINSTALL_FILES) $(OBJS)

EXTRA_DIST = cache.c cache_.h

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
