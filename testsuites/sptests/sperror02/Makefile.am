
rtems_tests_PROGRAMS = sperror02
sperror02_SOURCES = init.c

dist_rtems_tests_DATA = sperror02.scn
dist_rtems_tests_DATA += sperror02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sperror02_OBJECTS)
LINK_LIBS = $(sperror02_LDLIBS)

sperror02$(EXEEXT): $(sperror02_OBJECTS) $(sperror02_DEPENDENCIES)
	@rm -f sperror02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
