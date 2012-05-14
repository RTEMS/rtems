
rtems_tests_PROGRAMS = termios08
termios08_SOURCES = init.c ../termios03/termios_testdriver_polled.c

dist_rtems_tests_DATA = termios08.scn
dist_rtems_tests_DATA += termios08.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/termios03

LINK_OBJS = $(termios08_OBJECTS)
LINK_LIBS = $(termios08_LDLIBS)

termios08$(EXEEXT): $(termios08_OBJECTS) $(termios08_DEPENDENCIES)
	@rm -f termios08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
