##
## $Id$
##

PGM = $(ARCH)/mmu.rel

C_FILES = bat.c pte121.c

S_FILES = mmuAsm.S

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = bat.h pte121.h

mmu_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT)) \
    $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(mmu_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(PREINSTALL_FILES) $(mmu_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = bat.c bat.h mmuAsm.S pte121.c pte121.h

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/bat.h: bat.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/bat.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/bat.h

$(PROJECT_INCLUDE)/libcpu/pte121.h: pte121.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/pte121.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/pte121.h

include $(top_srcdir)/../../../automake/local.am
