include $(top_srcdir)/automake/compile.am

if LIBDEBUGGER

project_lib_LIBRARIES = libdebugger.a
libdebugger_a_SOURCES = \
  rtems-debugger-block.c \
  rtems-debugger-cmd.c \
  rtems-debugger-remote.c \
  rtems-debugger-server.c \
  rtems-debugger-target.c \
  rtems-debugger-remote-tcp.c \
  rtems-debugger-threads.c \
  rtems-debugger-@RTEMS_CPU@.c

$(PROJECT_LIB)/libdebugger.a: libdebugger.a
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libdebugger.a
TMPINSTALL_FILES = $(PROJECT_LIB)/libdebugger.a

endif

include $(top_srcdir)/automake/local.am
