ACLOCAL_AMFLAGS = -I ../aclocal

include $(top_srcdir)/../automake/compile.am

if HAS_CXX
include_rtems__dir = $(includedir)/rtems++

include_rtems___HEADERS = include/rtems++/rtemsEvent.h \
    include/rtems++/rtemsInterrupt.h include/rtems++/rtemsMessageQueue.h \
    include/rtems++/rtemsSemaphore.h include/rtems++/rtemsStatusCode.h \
    include/rtems++/rtemsTask.h include/rtems++/rtemsTaskMode.h \
    include/rtems++/rtemsTimer.h

project_lib_LIBRARIES = librtems++.a

librtems___a_SOURCES = src/rtemsEvent.cc src/rtemsInterrupt.cc \
    src/rtemsMessageQueue.cc src/rtemsSemaphore.cc src/rtemsStatusCode.cc \
    src/rtemsTask.cc src/rtemsTimer.cc
librtems___a_CPPFLAGS = $(AM_CPPFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../automake/local.am
