##
## $Id$
##

C_FILES = cpuIdent.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = spr.h io.h mmu.h page.h byteorder.h pgtable.h \
    cpuIdent.h

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

LIB = $(ARCH)/libcpuspec.a

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = cpuIdent.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/spr.h: spr.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/spr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/spr.h

$(PROJECT_INCLUDE)/libcpu/io.h: io.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/io.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/io.h

$(PROJECT_INCLUDE)/libcpu/mmu.h: mmu.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/mmu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/mmu.h

$(PROJECT_INCLUDE)/libcpu/page.h: page.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/page.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/page.h

$(PROJECT_INCLUDE)/libcpu/byteorder.h: byteorder.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/byteorder.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/byteorder.h

$(PROJECT_INCLUDE)/libcpu/pgtable.h: pgtable.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/pgtable.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/pgtable.h

$(PROJECT_INCLUDE)/libcpu/cpuIdent.h: cpuIdent.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/cpuIdent.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/cpuIdent.h

include $(top_srcdir)/../../../automake/local.am
