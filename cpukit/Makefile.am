ACLOCAL_AMFLAGS = -I aclocal

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am

noinst_LIBRARIES = libcpukit.a

libcpukit_a_SOURCES =
libcpukit_a_SOURCES += sapi/src/chainappendnotify.c
libcpukit_a_SOURCES += sapi/src/chaingetnotify.c
libcpukit_a_SOURCES += sapi/src/chaingetwait.c
libcpukit_a_SOURCES += sapi/src/chainprependnotify.c
libcpukit_a_SOURCES += sapi/src/chainprotected.c
libcpukit_a_SOURCES += sapi/src/cpucounterconverter.c
libcpukit_a_SOURCES += sapi/src/delaynano.c
libcpukit_a_SOURCES += sapi/src/delayticks.c
libcpukit_a_SOURCES += sapi/src/exinit.c
libcpukit_a_SOURCES += sapi/src/exshutdown.c
libcpukit_a_SOURCES += sapi/src/extension.c
libcpukit_a_SOURCES += sapi/src/extensioncreate.c
libcpukit_a_SOURCES += sapi/src/extensiondelete.c
libcpukit_a_SOURCES += sapi/src/extensionident.c
libcpukit_a_SOURCES += sapi/src/fatal.c
libcpukit_a_SOURCES += sapi/src/fatalsrctext.c
libcpukit_a_SOURCES += sapi/src/getversionstring.c
libcpukit_a_SOURCES += sapi/src/interrtext.c
libcpukit_a_SOURCES += sapi/src/io.c
libcpukit_a_SOURCES += sapi/src/ioclose.c
libcpukit_a_SOURCES += sapi/src/iocontrol.c
libcpukit_a_SOURCES += sapi/src/ioinitialize.c
libcpukit_a_SOURCES += sapi/src/ioopen.c
libcpukit_a_SOURCES += sapi/src/ioread.c
libcpukit_a_SOURCES += sapi/src/ioregisterdriver.c
libcpukit_a_SOURCES += sapi/src/iounregisterdriver.c
libcpukit_a_SOURCES += sapi/src/iowrite.c
libcpukit_a_SOURCES += sapi/src/panic.c
libcpukit_a_SOURCES += sapi/src/posixapi.c
libcpukit_a_SOURCES += sapi/src/profilingiterate.c
libcpukit_a_SOURCES += sapi/src/profilingreportxml.c
libcpukit_a_SOURCES += sapi/src/rbheap.c
libcpukit_a_SOURCES += sapi/src/rbtree.c
libcpukit_a_SOURCES += sapi/src/rbtreefind.c
libcpukit_a_SOURCES += sapi/src/sapirbtreeinsert.c
libcpukit_a_SOURCES += sapi/src/tcsimpleinstall.c
libcpukit_a_SOURCES += sapi/src/version.c

#
# Create a new Version VC Key header if the VC state has changed.
#
stamp_vc_key = stamp-vc-key

BUILT_SOURCES = version-vc-key.h

.PHONY: generate-vc-key

generate-vc-key:
	@+current_vc_key=""; \
	if test -f $(stamp_vc_key); then \
	 current_vc_key=`cat $(stamp_vc_key)`; \
	fi; \
	vc_key=`$(top_srcdir)/vc-key.sh $(top_srcdir) $$current_vc_key`; \
	if test "$$vc_key" != "matches"; then \
	 echo "Generating version-vc-key.h"; \
	 if test "$$vc_key" == "release"; then \
	  vc_header_key="\/\* No version control key found; release\? \*\/"; \
	 else \
	  vc_header_key="#define RTEMS_VERSION_VC_KEY \"$$vc_key\""; \
	 fi; \
	 cat $(top_srcdir)/version-vc-key.h.in | \
	  sed -e "s/@VERSION_VC_KEY@/$$vc_header_key/g" > version-vc-key.h; \
	 echo "$$vc_key" > $(stamp_vc_key); \
	fi

version-vc-key.h: generate-vc-key

$(top_srcdir)/sapi/src/version.c: version-vc-key.h

all-local: generate-vc-key

# librtemscpu
_SUBDIRS = . score rtems posix
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
