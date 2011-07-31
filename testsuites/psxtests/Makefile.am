##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = psxclock
if HAS_POSIX
SUBDIRS += psxhdrs psx01 psx02 psx03 psx04 psx05 psx06 psx07 psx08 psx09 \
    psx10 psx11 psx12 psx13 psx14 psx16 psxalarm01 \
    psxautoinit01 psxautoinit02 psxbarrier01 \
    psxcancel psxcancel01 psxcleanup psxcleanup01 \
    psxcond01 psxenosys psxkey01 psxkey02 \
    psxkey03 psxitimer psxmsgq01 psxmsgq02 psxmsgq03 psxmsgq04 \
    psxmutexattr01 psxobj01 psxrwlock01 psxsem01 psxsignal01 psxsignal02 \
    psxsignal03 psxsignal04 psxsignal05 psxspin01 psxspin02 psxsysconf \
    psxtime psxtimer01 psxtimer02 psxualarm psxusleep psxfatal01 psxfatal02 \
    psxintrcritical01 psxstack01 psxeintr_join
endif

## File IO tests
SUBDIRS += psxfile01 psxreaddir psxstat psxmount psx13 psxchroot01 \
  psxpasswd01

## Until sys/uio.h is moved to libcsupport, we have to have networking
## enabled to support readv and writev.  Hopefully this is a temporary
## situation.
## if NEWLIB
SUBDIRS += psxrdwrv
## endif

DIST_SUBDIRS = $(SUBDIRS) psxfatal_support
EXTRA_DIST = psxfatal_support/init.c psxfatal_support/system.h

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
