##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools start startup clock console timer ne2000 wd8003 3c509 ide \
    wrapup

include $(top_srcdir)/../../bsp.am

# This driver needs to be reworked for the BSD stack.
# We only install wd80x3.h if HAS_NETWORKING was defined
if HAS_NETWORKING
include_HEADERS += include/wd80x3.h
endif

include_HEADERS += include/crt.h
nodist_include_HEADERS += $(top_srcdir)/../../shared/include/coverhd.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = HOWTO README.dec21140 STATUS times_i486dx times_p5

if HAS_NETWORKING
$(PROJECT_INCLUDE)/wd80x3.h: include/wd80x3.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/wd80x3.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/wd80x3.h
endif

$(PROJECT_INCLUDE)/crt.h: include/crt.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/crt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/crt.h

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
