##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

SUBDIRS = duart vectorisrs

include_mipsdir = $(includedir)/libcpu
include_mips_HEADERS = include/mongoose-v.h

all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libcpu
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu

$(PROJECT_INCLUDE)/libcpu/mongoose-v.h: include/mongoose-v.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/mongoose-v.h

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
