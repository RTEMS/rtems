include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/confdefs.h
include_rtems_HEADERS += include/rtems/chain.h
include_rtems_HEADERS += include/rtems/config.h
include_rtems_HEADERS += include/rtems/extension.h
include_rtems_HEADERS += include/rtems/fatal.h
include_rtems_HEADERS += include/rtems/init.h
include_rtems_HEADERS += include/rtems/io.h
include_rtems_HEADERS += include/rtems/mptables.h
include_rtems_HEADERS += include/rtems/cbs.h
include_rtems_HEADERS += include/rtems/rbheap.h
include_rtems_HEADERS += include/rtems/rbtree.h
include_rtems_HEADERS += include/rtems/sptables.h

EXTRA_DIST = include/rtems/README

include_rtems_HEADERS += inline/rtems/chain.inl
include_rtems_HEADERS += inline/rtems/extension.inl
include_rtems_HEADERS += inline/rtems/cbs.inl
include_rtems_HEADERS += inline/rtems/rbtree.inl

## src
AM_CPPFLAGS += -D__RTEMS_INSIDE__

noinst_LIBRARIES = libsapi.a
project_lib_LIBRARIES = libsapi.a
libsapi_a_SOURCES = src/debug.c src/extension.c src/extensioncreate.c \
    src/extensiondelete.c src/extensionident.c src/fatal.c src/exinit.c \
    src/exshutdown.c src/io.c src/ioclose.c src/iocontrol.c src/iodata.c \
    src/ioinitialize.c src/ioopen.c src/ioread.c src/ioregisterdriver.c \
    src/iounregisterdriver.c src/iowrite.c src/posixapi.c  \
    src/rtemsapi.c src/extensiondata.c src/getversionstring.c \
    src/chainappendnotify.c src/chaingetnotify.c src/chaingetwait.c \
    src/chainprependnotify.c src/rbheap.c
libsapi_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
