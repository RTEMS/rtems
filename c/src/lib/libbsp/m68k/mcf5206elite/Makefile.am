##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories

SUBDIRS = . start startup console i2c tod nvram tools wrapup

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/coverhd.h
include_HEADERS += include/i2c.h
include_HEADERS += include/i2cdrv.h
include_HEADERS += include/ds1307.h
include_HEADERS += include/nvram.h
nodist_include_HEADERS += $(top_srcdir)/../../shared/tod.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = times

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_INCLUDE)/i2c.h: include/i2c.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/i2c.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/i2c.h

$(PROJECT_INCLUDE)/i2cdrv.h: include/i2cdrv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/i2cdrv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/i2cdrv.h

$(PROJECT_INCLUDE)/ds1307.h: include/ds1307.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ds1307.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ds1307.h

$(PROJECT_INCLUDE)/nvram.h: include/nvram.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/nvram.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nvram.h

$(PROJECT_INCLUDE)/tod.h: $(top_srcdir)/../../shared/tod.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tod.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tod.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
