
rtems_tests_PROGRAMS = termios07
termios07_SOURCES = init.c ../termios04/termios_testdriver_intr.c

dist_rtems_tests_DATA = termios07.scn
dist_rtems_tests_DATA += termios07.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/termios04

LINK_OBJS = $(termios07_OBJECTS)
LINK_LIBS = $(termios07_LDLIBS)

termios07$(EXEEXT): $(termios07_OBJECTS) $(termios07_DEPENDENCIES)
	@rm -f termios07$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
