##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

## wrapup is the one that actually builds and installs the library
## from the individual .rel files built in other directories
SUBDIRS = . start startup console clock timer wrapup
SUBDIRS += tools

# FIXME: This directory is missing
# ramdisk

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/syscall.h
include_HEADERS += include/ramdisk.h
include_HEADERS += include/coverhd.h

all-local: $(PREINSTALL_FILES)

$(PROJECT_INCLUDE)/syscall.h: include/syscall.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/syscall.h

$(PROJECT_INCLUDE)/ramdisk.h: include/ramdisk.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ramdisk.h

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
