##
## $Id$
##


PGM = $(ARCH)/mmu.rel

C_FILES = bat.c pte121.c

S_FILES = mmuAsm.S

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = bat.h pte121.h

mmu_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT)) $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(mmu_rel_OBJECTS)
	$(make-rel)

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libcpu \
    $(include_libcpu_HEADERS:%=$(PROJECT_INCLUDE)/libcpu/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(mmu_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = bat.c bat.h mmuAsm.S pte121.c pte121.h

include $(top_srcdir)/../../../../../automake/local.am
