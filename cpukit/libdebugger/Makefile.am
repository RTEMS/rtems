if LIBDEBUGGER

include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libdebugger.a
libdebugger_a_SOURCES = \
  rtems-debugger-block.c \
  rtems-debugger-cmd.c \
  rtems-debugger-remote.c \
  rtems-debugger-server.c \
  rtems-debugger-target.c \
  rtems-debugger-threads.c \
  rtems-debugger-@RTEMS_CPU@.c

if LIBNETWORKING
libdebugger_a_SOURCES += \
  rtems-debugger-remote-tcp.c
endif

endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
