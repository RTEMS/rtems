
rtems_tests_PROGRAMS = uid01
uid01_SOURCES = init.c serial_mouse_config.c msmouse.c\
    ../termios04/termios_testdriver_intr.c

dist_rtems_tests_DATA = uid01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/termios04

LINK_OBJS = $(uid01_OBJECTS)
LINK_LIBS = $(uid01_LDLIBS)

uid01$(EXEEXT): $(uid01_OBJECTS) $(uid01_DEPENDENCIES)
	@rm -f uid01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
