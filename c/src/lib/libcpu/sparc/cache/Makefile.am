##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/src

C_FILES = cache.c cache_aligned_malloc.c cache_manager.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = cache.h
noinst_HEADERS = cache_.h

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

if has_instruction_cache
CACHE_DEFINE = -DHAS_INSTRUCTION_CACHE
endif

AM_CPPFLAGS += -I$(srcdir) $(CACHE_DEFINE)

cache.h: $(top_srcdir)/../shared/include/cache.h
	cp $< $@

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

CLEANFILES = cache.h

EXTRA_DIST = cache.c cache_.h

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/cache.h: cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

include $(top_srcdir)/../../../automake/local.am
