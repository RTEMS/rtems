##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = i960CA.h i960HA.h i960KA.h i960RP.h i960JX_RP_common.h

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu \
    $(include_libcpu_HEADERS:%=$(PROJECT_INCLUDE)/libcpu/%)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = i960CA.h i960HA.h i960KA.h i960RP.h i960JX_RP_common.h

include $(top_srcdir)/../../../../../automake/local.am
