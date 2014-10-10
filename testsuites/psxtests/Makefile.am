ACLOCAL_AMFLAGS = -I ../aclocal

_SUBDIRS = psxclock
if HAS_POSIX
_SUBDIRS += psxhdrs psx01 psx02 psx03 psx04 psx05 psx06 psx07 psx08 psx09 \
    psx10 psx11 psx12 psx13 psx14 psx15 psx16 \
    psxaio01 psxaio02 psxaio03 \
    psxalarm01 psxautoinit01 psxautoinit02 psxbarrier01 \
    psxcancel psxcancel01 psxclassic01 psxcleanup psxcleanup01 \
    psxcond01 psxconfig01 psxenosys \
    psxitimer psxmsgq01 psxmsgq02 psxmsgq03 psxmsgq04 \
    psxmutexattr01 psxobj01 psxrwlock01 psxsem01 psxsignal01 psxsignal02 \
    psxsignal03 psxsignal04 psxsignal05 psxsignal06 \
    psxspin01 psxspin02 psxsysconf \
    psxtime psxtimer01 psxtimer02 psxualarm psxusleep psxfatal01 psxfatal02 \
    psxintrcritical01 psxstack01 psxstack02 \
    psxeintr_join psxgetattrnp01
if HAS_CPLUSPLUS
_SUBDIRS += psxglobalcon01
_SUBDIRS += psxglobalcon02
endif
endif

## File IO tests
_SUBDIRS += psxfile01 psxfile02 psxfilelock01 psxgetrusage01 psxid01 \
    psximfs01 psximfs02 psxreaddir psxstat psxmount psx13 psxchroot01 \
    psxpasswd01 psxpasswd02 psxpipe01 psxtimes01 psxfchx01

## POSIX Keys are always available
_SUBDIRS += psxkey01 psxkey02 psxkey03 psxkey04 \
    psxkey05 psxkey06 psxkey08 psxkey09 psxkey10
## But some of the tests need other POSIX features
if HAS_POSIX
_SUBDIRS += psxkey07
endif

## POSIX Once is always available
_SUBDIRS += psxonce01

_SUBDIRS += psxrdwrv

include $(top_srcdir)/../automake/test-subdirs.am
include $(top_srcdir)/../automake/local.am
