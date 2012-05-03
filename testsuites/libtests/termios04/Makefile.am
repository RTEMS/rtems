
rtems_tests_PROGRAMS = termios04
termios04_SOURCES = init.c termios_testdriver_intr.c
termios04_SOURCES += termios_testdriver_intr.h

dist_rtems_tests_DATA = termios04.scn
dist_rtems_tests_DATA += termios04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(termios04_OBJECTS)
LINK_LIBS = $(termios04_LDLIBS)

termios04$(EXEEXT): $(termios04_OBJECTS) $(termios04_DEPENDENCIES)
	@rm -f termios04$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
