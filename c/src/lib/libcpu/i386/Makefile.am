##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

VPATH = @srcdir@:@srcdir@/../shared/src

C_FILES = cache.c cache_aligned_malloc.c cache_manager.c displayCpu.c \
    page.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_libcpudir = $(includedir)/libcpu

noinst_HEADERS = cache_.h
include_libcpu_HEADERS = cpu.h cpuModel.h cache.h

S_FILES = cpuModel.S idtr.S
OBJS += $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

AM_CPPFLAGS += -I$(srcdir)

cache.h: $(top_srcdir)/../shared/include/cache.h
	cp $< $@

all-local: $(PREINSTALL_FILES) $(OBJS)

CLEANFILES = cache.h

EXTRA_DIST = cache.c cache_.h cpu.h cpuModel.S cpuModel.h displayCpu.c idt.c \
    idtr.S page.c registers.h

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/cpu.h: cpu.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cpu.h

$(PROJECT_INCLUDE)/libcpu/cpuModel.h: cpuModel.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cpuModel.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cpuModel.h

$(PROJECT_INCLUDE)/libcpu/cache.h: cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

include $(top_srcdir)/../../../automake/local.am
