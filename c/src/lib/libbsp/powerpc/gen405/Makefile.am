##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = . include dlentry startup wrapup

include $(top_srcdir)/../../bsp.am

$(PROJECT_RELEASE)/lib/bsp_specs: bsp_specs
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_RELEASE)/lib/bsp_specs

EXTRA_DIST = bsp_specs times

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
