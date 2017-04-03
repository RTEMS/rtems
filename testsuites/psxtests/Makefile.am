ACLOCAL_AMFLAGS = -I ../aclocal

## Some of the tests need other POSIX features to work while some do not.

_SUBDIRS  =
if HAS_POSIX
_SUBDIRS += psx01
_SUBDIRS += psx02
_SUBDIRS += psx03
_SUBDIRS += psx04
_SUBDIRS += psx05
_SUBDIRS += psx06
_SUBDIRS += psx07
_SUBDIRS += psx08
_SUBDIRS += psx09
_SUBDIRS += psx10
_SUBDIRS += psx11
_SUBDIRS += psx12
_SUBDIRS += psx14
_SUBDIRS += psx15
_SUBDIRS += psx16
_SUBDIRS += psxaio01
_SUBDIRS += psxaio02
_SUBDIRS += psxaio03
_SUBDIRS += psxalarm01
_SUBDIRS += psxautoinit01
_SUBDIRS += psxautoinit02
_SUBDIRS += psxbarrier01
_SUBDIRS += psxcancel
_SUBDIRS += psxcancel01
_SUBDIRS += psxclassic01
_SUBDIRS += psxcleanup
_SUBDIRS += psxcleanup01
_SUBDIRS += psxcleanup02
endif
_SUBDIRS += psxclock
if HAS_POSIX
_SUBDIRS += psxclock01
_SUBDIRS += psxconcurrency01
_SUBDIRS += psxcond01
_SUBDIRS += psxcond02
_SUBDIRS += psxconfig01
endif
_SUBDIRS += psxdevctl01
if HAS_POSIX
_SUBDIRS += psxeintr_join
_SUBDIRS += psxenosys
_SUBDIRS += psxfatal01
_SUBDIRS += psxfatal02
endif
if HAS_POSIX
if HAS_CPLUSPLUS
_SUBDIRS += psxglobalcon01
_SUBDIRS += psxglobalcon02
endif
_SUBDIRS += psxhdrs
_SUBDIRS += psxintrcritical01
_SUBDIRS += psxitimer
endif
_SUBDIRS += psxkey01
_SUBDIRS += psxkey02
_SUBDIRS += psxkey03
_SUBDIRS += psxkey04
_SUBDIRS += psxkey05
_SUBDIRS += psxkey06
if HAS_POSIX
_SUBDIRS += psxkey07
endif
_SUBDIRS += psxkey08
_SUBDIRS += psxkey09
_SUBDIRS += psxkey10
if HAS_POSIX
_SUBDIRS += psxmsgq01
_SUBDIRS += psxmsgq02
_SUBDIRS += psxmsgq03
_SUBDIRS += psxmsgq04
_SUBDIRS += psxmutexattr01
_SUBDIRS += psxobj01
endif
_SUBDIRS += psxonce01
_SUBDIRS += psxrdwrv
if HAS_POSIX
_SUBDIRS += psxrwlock01
_SUBDIRS += psxsem01
_SUBDIRS += psxshm01
_SUBDIRS += psxshm02
_SUBDIRS += psxsignal01
_SUBDIRS += psxsignal02
_SUBDIRS += psxsignal03
_SUBDIRS += psxsignal04
_SUBDIRS += psxsignal05
_SUBDIRS += psxsignal06
_SUBDIRS += psxspin01
_SUBDIRS += psxstack01
_SUBDIRS += psxstack02
_SUBDIRS += psxstrsignal01
_SUBDIRS += psxsysconf
endif
_SUBDIRS += psxthreadname01
if HAS_POSIX
_SUBDIRS += psxtime
_SUBDIRS += psxtimer01
_SUBDIRS += psxtimer02
_SUBDIRS += psxualarm
_SUBDIRS += psxusleep
endif



## File IO tests
_SUBDIRS += psx13
_SUBDIRS += psxchroot01
_SUBDIRS += psxfile01
_SUBDIRS += psxfile02
_SUBDIRS += psxfilelock01
_SUBDIRS += psxgetattrnp01
_SUBDIRS += psxgetrusage01
_SUBDIRS += psxid01
_SUBDIRS += psximfs01
_SUBDIRS += psximfs02
_SUBDIRS += psxmount
_SUBDIRS += psxpasswd01
_SUBDIRS += psxpasswd02
_SUBDIRS += psxpipe01
_SUBDIRS += psxreaddir
_SUBDIRS += psxstat
_SUBDIRS += psxtimes01
_SUBDIRS += psxfchx01

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
