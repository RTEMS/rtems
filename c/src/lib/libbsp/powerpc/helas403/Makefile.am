##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . dlentry flashentry startup @exceptions@ wrapup

include $(top_srcdir)/../../bsp.am

dist_bsplib_DATA += bsp_specs.dl

include_HEADERS += include/coverhd.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = times

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_ROOT)/@RTEMS_BSP@/lib/bsp_specs.dl: bsp_specs.dl
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/lib/bsp_specs.dl

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
