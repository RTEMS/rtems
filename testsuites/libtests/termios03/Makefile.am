
rtems_tests_PROGRAMS = termios03
termios03_SOURCES = init.c termios_testdriver_polled.c
termios03_SOURCES += termios_testdriver_polled.h

dist_rtems_tests_DATA = termios03.scn
dist_rtems_tests_DATA += termios03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(termios03_OBJECTS)
LINK_LIBS = $(termios03_LDLIBS)

termios03$(EXEEXT): $(termios03_OBJECTS) $(termios03_DEPENDENCIES)
	@rm -f termios03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
