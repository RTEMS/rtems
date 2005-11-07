##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

noinst_HEADERS = include/pmacros.h

POSIX_DIRS = psxhdrs psx01 psx02 psx03 psx04 psx05 psx06 psx07 psx08 psx09 \
    psx10 psx11 psx12 psxtime psxtimer psxcancel psxmsgq01 psxsem01

POSIX_FILES_DIRS = psxfile01 psxreaddir psxstat psxmount psx13 psxchroot01

SUBDIRS = $(POSIX_DIRS) $(POSIX_FILES_DIRS)

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
