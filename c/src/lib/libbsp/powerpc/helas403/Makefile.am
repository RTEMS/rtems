##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = include dlentry flashentry startup @exceptions@ wrapup

include $(top_srcdir)/../../bsp.am

$(PROJECT_RELEASE)/lib/bsp_specs.dl: bsp_specs.dl
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_RELEASE)/lib/bsp_specs.dl

EXTRA_DIST = bsp_specs bsp_specs.dl times

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
