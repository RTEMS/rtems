##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

VPATH = @srcdir@:@srcdir@/../shared/src

C_FILES = cache.c cache_aligned_malloc.c cache_manager.c displayCpu.c page.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_libcpudir = $(includedir)/libcpu

noinst_HEADERS = cache_.h
include_libcpu_HEADERS = cpu.h cpuModel.h cache.h

S_FILES = cpuModel.S idtr.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

AM_CPPFLAGS += -I$(srcdir)

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

EXTRA_DIST = cache.c cache_.h cpu.h cpuModel.S cpuModel.h displayCpu.c idt.c \
    idtr.S page.c registers.h

include $(top_srcdir)/../../../../../automake/local.am
