##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared/src

C_FILES = cache.c cache_aligned_malloc.c cache_manager.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = cache.h
noinst_HEADERS = cache_.h

include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

if has_instruction_cache
CACHE_DEFINE = -DHAS_INSTRUCTION_CACHE
endif

AM_CPPFLAGS += -I$(srcdir) $(CACHE_DEFINE)

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

cache.h: $(top_srcdir)/../shared/include/cache.h
	cp $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libcpu \
    $(include_libcpu_HEADERS:%=$(PROJECT_INCLUDE)/libcpu/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

CLEANFILES = cache.h

EXTRA_DIST = cache.c cache_.h

include $(top_srcdir)/../../../../../automake/local.am
