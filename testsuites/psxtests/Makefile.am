##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = psxhdrs psx01 psx02 psx03 psx04 psx05 psx06 psx07 psx08 psx09 \
    psx10 psx11 psx12 psx13 psx14 psxautoinit01 psxautoinit02 psxbarrier01 \
    psxcancel psxcleanup psxenosys psxfatal01 psxfatal02 psxkey01 psxitimer \
    psxmsgq01 psxmsgq02 psxmsgq03 psxmutexattr01 psxobj01 psxrwlock01 \
    psxsem01 psxsignal01 psxspin01 psxspin02 psxsysconf psxtime psxtimer01 \
    psxtimer02 psxualarm 

## File IO tests
SUBDIRS += psxfile01 psxreaddir psxstat psxmount psx13 psxchroot01 
## Until sys/uio.h is moved to libcsupport, we have to have networking
## enabled to support readv and writev.  Hopefully this is a temporary
## situation.
if HAS_NETWORKING
SUBDIRS += psxrdwrv
endif

DIST_SUBDIRS = $(SUBDIRS) psxfatal_support
EXTRA_DIST = psxfatal_support/init.c psxfatal_support/system.h

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
