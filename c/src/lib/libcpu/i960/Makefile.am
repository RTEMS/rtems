##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

SUBDIRS = . i960ca i960ka i960rp

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = include/i960CA.h include/i960HA.h include/i960KA.h \
    include/i960RP.h include/i960JX_RP_common.h

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: include/%.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_INCLUDE)/libcpu \
    $(include_libcpu_HEADERS:include/%.h=$(PROJECT_INCLUDE)/libcpu/%)

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
