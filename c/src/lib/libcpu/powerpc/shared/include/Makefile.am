##
## $Id$
##


C_FILES = cpuIdent.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = cpu.h spr.h io.h mmu.h page.h byteorder.h pgtable.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

LIB = $(ARCH)/libcpuspec.a

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libcpu \
    $(include_libcpu_HEADERS:%=$(PROJECT_INCLUDE)/libcpu/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = byteorder.h cpu.h cpuIdent.c io.h mmu.h page.h pgtable.h spr.h

include $(top_srcdir)/../../../../../automake/local.am
