##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if need_shared
SUBDIRS = clock console include pci residual openpic irq vectors start \
    startup motorola bootloader
endif

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
