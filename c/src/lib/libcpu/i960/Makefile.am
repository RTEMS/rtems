##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

SUBDIRS = . i960ca i960ka i960rp

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = include/i960CA.h include/i960HA.h include/i960KA.h \
    include/i960RP.h include/i960JX_RP_common.h

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/i960CA.h: include/i960CA.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/i960CA.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/i960CA.h

$(PROJECT_INCLUDE)/libcpu/i960HA.h: include/i960HA.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/i960HA.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/i960HA.h

$(PROJECT_INCLUDE)/libcpu/i960KA.h: include/i960KA.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/i960KA.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/i960KA.h

$(PROJECT_INCLUDE)/libcpu/i960RP.h: include/i960RP.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/i960RP.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/i960RP.h

$(PROJECT_INCLUDE)/libcpu/i960JX_RP_common.h: include/i960JX_RP_common.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/i960JX_RP_common.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/i960JX_RP_common.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
