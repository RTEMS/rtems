##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
if need_motorola_powerpc
SUBDIRS = include clock console pci residual openpic irq vectors start \
    startup bootloader motorola wrapup
endif

include $(top_srcdir)/../../bsp.am

EXTRA_DIST = BOOTING README.MVME2300 README.OTHERBOARDS README.dec21140 \
    bsp_specs times.mcp750 times.mvme2307

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
