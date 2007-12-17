##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = psxhdrs psx01 psx02 psx03 psx04 psx05 psx06 psx07 psx08 psx09 \
    psx10 psx11 psx12 psxtime psxtimer01 psxtimer02  psxcancel psxbarrier01 \
    psxmsgq01 psxrwlock01 psxsem01 psxspin01 psxenosys

## File IO tests
SUBDIRS += psxfile01 psxreaddir psxstat psxmount psx13 psxchroot01

## Until sys/uio.h is moved to libcsupport, we have to have networking
## enabled to support readv and writev.  Hopefully this is a temporary
## situation.
if HAS_NETWORKING
SUBDIRS += psxrdwrv
endif


include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
