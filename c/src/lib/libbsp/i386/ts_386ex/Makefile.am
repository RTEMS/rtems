##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools startup clock console timer network wrapup

if HAS_NETWORKING
include_HEADERS += include/wd80x3.h
endif

include_HEADERS += include/coverhd.h

noinst_HEADERS = start/80386ex.h start/80386ex.inc start/macros.inc \
    start/ts_1325.inc
S_FILES = start/start.S
$(ARCH)/%$(LIB_VARIANT).$(OBJEXT): start/start.S $(ARCH)/$(dirstamp)
	$(CCASCOMPILE) -DASM -o $@ -c $<

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

if HAS_NETWORKING
$(PROJECT_INCLUDE)/wd80x3.h: include/wd80x3.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/wd80x3.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/wd80x3.h
endif

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

CLEANFILES = $(PREINSTALL_FILES)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
