rtems_tests_PROGRAMS = math
math_SOURCES = init.c domath.c

dist_rtems_tests_DATA = math.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

math_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/math
math_LDADD = -lm

LINK_OBJS = $(math_OBJECTS) $(math_LDADD)
LINK_LIBS = $(math_LDLIBS)

math$(EXEEXT): $(math_OBJECTS) $(math_DEPENDENCIES)
	@rm -f math$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
