##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

SUBDIRS = duart vectorisrs

include_mipsdir = $(includedir)/libcpu
include_mips_HEADERS = include/mongoose-v.h

$(PROJECT_INCLUDE)/libcpu:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/libcpu/%.h: include/%.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/libcpu \
    $(include_mips_HEADERS:include/%.h=$(PROJECT_INCLUDE)/libcpu/%.h)

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
