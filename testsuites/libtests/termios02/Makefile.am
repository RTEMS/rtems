
rtems_tests_PROGRAMS = termios02
termios02_SOURCES = init.c

dist_rtems_tests_DATA = termios02.scn
dist_rtems_tests_DATA += termios02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(termios02_OBJECTS)
LINK_LIBS = $(termios02_LDLIBS)

termios02$(EXEEXT): $(termios02_OBJECTS) $(termios02_DEPENDENCIES)
	@rm -f termios02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
