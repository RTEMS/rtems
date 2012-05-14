ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = psxclock
if HAS_POSIX
SUBDIRS += psxhdrs psx01 psx02 psx03 psx04 psx05 psx06 psx07 psx08 psx09 \
    psx10 psx11 psx12 psx13 psx14 psx15 psx16 \
    psxaio01 psxaio02 psxaio03 \
    psxalarm01 psxautoinit01 psxautoinit02 psxbarrier01 \
    psxcancel psxcancel01 psxclassic01 psxcleanup psxcleanup01 \
    psxcond01 psxconfig01 psxenosys psxkey01 psxkey02 psxkey03 \
    psxitimer psxmsgq01 psxmsgq02 psxmsgq03 psxmsgq04 \
    psxmutexattr01 psxobj01 psxrwlock01 psxsem01 psxsignal01 psxsignal02 \
    psxsignal03 psxsignal04 psxsignal05 psxsignal06 \
    psxspin01 psxspin02 psxsysconf \
    psxtime psxtimer01 psxtimer02 psxualarm psxusleep psxfatal01 psxfatal02 \
    psxintrcritical01 psxstack01 psxstack02 \
    psxeintr_join
endif

## File IO tests
SUBDIRS += psxfile01 psxfile02 psxfilelock01 psxgetrusage01 psxid01 \
    psximfs01 psximfs02 psxreaddir psxstat psxmount psx13 psxchroot01 \
    psxpasswd01 psxpasswd02 psxpipe01 psxtimes01 psxfchx01

SUBDIRS += psxrdwrv

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
