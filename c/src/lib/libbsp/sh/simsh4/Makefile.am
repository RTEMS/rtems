##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

## wrapup is the one that actually builds and installs the library
## from the individual .rel files built in other directories
SUBDIRS = . startup console clock timer wrapup
SUBDIRS += tools

# FIXME: This directory is missing
# ramdisk

include_HEADERS += include/syscall.h
include_HEADERS += include/ramdisk.h
include_HEADERS += include/coverhd.h

S_FILES = start/start.S
$(ARCH)/%$(LIB_VARIANT).$(OBJEXT): start/start.S $(ARCH)/$(dirstamp)
	${CCASCOMPILE} -DASM -o $@ -c $<

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST = $(S_FILES)

$(PROJECT_INCLUDE)/syscall.h: include/syscall.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/syscall.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/syscall.h

$(PROJECT_INCLUDE)/ramdisk.h: include/ramdisk.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ramdisk.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ramdisk.h

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
