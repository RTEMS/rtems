
rtems_tests_PROGRAMS = termios05
termios05_SOURCES = init.c ../termios04/termios_testdriver_intr.c

dist_rtems_tests_DATA = termios05.scn
dist_rtems_tests_DATA += termios05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/termios04
AM_CPPFLAGS += -DTASK_DRIVEN

LINK_OBJS = $(termios05_OBJECTS)
LINK_LIBS = $(termios05_LDLIBS)

termios05$(EXEEXT): $(termios05_OBJECTS) $(termios05_DEPENDENCIES)
	@rm -f termios05$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
