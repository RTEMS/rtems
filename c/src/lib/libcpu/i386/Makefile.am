##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

VPATH = @srcdir@:@srcdir@/../shared/src

C_FILES = cache.c cache_aligned_malloc.c cache_manager.c displayCpu.c idt.c \
    page.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

H_FILES = cache_.h
INSTALLED_H_FILES = cpu.h registers.h cpuModel.h

S_FILES = cpuModel.S idtr.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

AM_CPPFLAGS += -I$(srcdir)

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/libcpu/cache.h: $(top_srcdir)/../shared/include/cache.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu \
    $(PROJECT_INCLUDE)/libcpu/cache.h \
    $(INSTALLED_H_FILES:%=$(PROJECT_INCLUDE)/libcpu/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

EXTRA_DIST = cache.c cache_.h cpu.h cpuModel.S cpuModel.h displayCpu.c idt.c \
    idtr.S page.c registers.h

include $(top_srcdir)/../../../../../automake/local.am
