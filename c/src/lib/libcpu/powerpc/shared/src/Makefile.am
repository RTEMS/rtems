##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../../shared/src

C_FILES = cache.c cache_aligned_malloc.c cache_manager.c stack.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = cache_.h cache.h stackTrace.h

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

AM_CPPFLAGS += -I$(srcdir)

cache.h: $(top_srcdir)/../shared/include/cache.h
	cp $< $@

all-local: $(PREINSTALL_FILES) $(OBJS)

CLEANFILES = cache.h

EXTRA_DIST = cache.c cache_.h

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/cache_.h: cache_.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache_.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache_.h

$(PROJECT_INCLUDE)/libcpu/cache.h: cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

$(PROJECT_INCLUDE)/libcpu/stackTrace.h: stackTrace.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/stackTrace.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/stackTrace.h

include $(top_srcdir)/../../../automake/local.am
