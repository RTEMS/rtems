##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

C_FILES = cache.c displayCpu.c page.c

common_C_FILES = ../shared/src/cache_aligned_malloc.c
common_C_FILES += ../shared/src/cache_manager.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))
OBJS += $(common_C_FILES:../shared/src/%.c=$(ARCH)/%.$(OBJEXT))

include_libcpudir = $(includedir)/libcpu

noinst_HEADERS = cache_.h
include_libcpu_HEADERS = cpu.h cpuModel.h
include_libcpu_HEADERS += ../shared/include/cache.h

S_FILES = cpuModel.S idtr.S
OBJS += $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): ../shared/src/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

AM_CPPFLAGS += -I$(srcdir)

all-local: $(PREINSTALL_FILES) $(OBJS)

EXTRA_DIST = cache.c cache_.h cpu.h cpuModel.S cpuModel.h displayCpu.c idt.c \
    idtr.S page.c registers.h

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/cpu.h: cpu.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cpu.h

$(PROJECT_INCLUDE)/libcpu/cpuModel.h: cpuModel.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cpuModel.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cpuModel.h

$(PROJECT_INCLUDE)/libcpu/cache.h: ../shared/include/cache.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cache.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
