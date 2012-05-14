
rtems_tests_PROGRAMS = termios06
termios06_SOURCES = init.c ../termios03/termios_testdriver_polled.c test_pppd.c

dist_rtems_tests_DATA = termios06.scn
dist_rtems_tests_DATA += termios06.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/termios03

LINK_OBJS = $(termios06_OBJECTS)
LINK_LIBS = $(termios06_LDLIBS)

termios06$(EXEEXT): $(termios06_OBJECTS) $(termios06_DEPENDENCIES)
	@rm -f termios06$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
