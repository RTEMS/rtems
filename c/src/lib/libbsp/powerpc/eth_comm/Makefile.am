##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . clock startup start canbus console irq network vectors \
    @exceptions@ wrapup

include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

include_HEADERS += include/coverhd.h
include_HEADERS += include/canbus.h
include_HEADERS += include/info.h
include_bsp_HEADERS  = include/commproc.h
include_bsp_HEADERS += include/8xx_immap.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = times

$(PROJECT_INCLUDE)/bsp:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/bsp
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp

$(PROJECT_INCLUDE)/bsp/commproc.h: include/commproc.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/commproc.h

$(PROJECT_INCLUDE)/bsp/8xx_immap.h: include/8xx_immap.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/8xx_immap.h

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_INCLUDE)/canbus.h: include/canbus.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/canbus.h

$(PROJECT_INCLUDE)/info.h: include/info.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/info.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
