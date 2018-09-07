ACLOCAL_AMFLAGS = -I aclocal

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am

# librtemscpu
_SUBDIRS = . score rtems sapi posix
_SUBDIRS += dev
_SUBDIRS += dtc/libfdt
_SUBDIRS += libcrypt
_SUBDIRS += libcsupport libblock libfs
_SUBDIRS += libdrvmgr
_SUBDIRS += libnetworking librpc
_SUBDIRS += libpci
_SUBDIRS += libi2c
_SUBDIRS += libmisc
_SUBDIRS += libmd
_SUBDIRS += libgnat
_SUBDIRS += libdl
_SUBDIRS += libstdthreads
_SUBDIRS += libdebugger
_SUBDIRS += wrapup

# other libraries
_SUBDIRS += zlib
_SUBDIRS += ftpd
_SUBDIRS += telnetd
_SUBDIRS += pppd
_SUBDIRS += mghttpd

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
