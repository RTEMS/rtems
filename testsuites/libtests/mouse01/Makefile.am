
rtems_tests_PROGRAMS = mouse01
mouse01_SOURCES = init.c serial_mouse_config.c msmouse.c\
    ../termios04/termios_testdriver_intr.c

dist_rtems_tests_DATA = mouse01.scn
dist_rtems_tests_DATA += mouse01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/termios04

LINK_OBJS = $(mouse01_OBJECTS)
LINK_LIBS = $(mouse01_LDLIBS)

mouse01$(EXEEXT): $(mouse01_OBJECTS) $(mouse01_DEPENDENCIES)
	@rm -f mouse01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
