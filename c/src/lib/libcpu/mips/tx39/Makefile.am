##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

SUBDIRS = . vectorisrs

include_libcpudir = $(includedir)/libcpu
include_libcpu_HEADERS = include/tx3904.h

all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/libcpu
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu

$(PROJECT_INCLUDE)/libcpu/tx3904.h: include/tx3904.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/tx3904.h

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
