rtems_tests_PROGRAMS = termios09
termios09_SOURCES = init.c

dist_rtems_tests_DATA = termios09.scn termios09.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(termios09_OBJECTS)
LINK_LIBS = $(termios09_LDLIBS)

termios09$(EXEEXT): $(termios09_OBJECTS) $(termios09_DEPENDENCIES)
	@rm -f termios09$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
