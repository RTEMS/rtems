##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = . include tools start startup clock console timer ne2000 wd8003 \
    3c509 wrapup

include $(top_srcdir)/../../bsp.am

$(PROJECT_RELEASE)/BootImgs:
	$(mkinstalldirs) $@

TMPINSTALL_FILES = $(PROJECT_RELEASE)/BootImgs

EXTRA_DIST = HOWTO README.dec21140 STATUS bsp_specs times_i486dx times_p5

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
