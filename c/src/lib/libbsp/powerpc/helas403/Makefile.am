##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . dlentry flashentry startup @exceptions@ wrapup

include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA += bsp_specs.dl

include_HEADERS += include/coverhd.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = times

TMPINSTALL_FILES =

${PROJECT_RELEASE}/lib/bsp_specs.dl: bsp_specs.dl ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/bsp_specs.dl
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/bsp_specs.dl

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
