
rtems_tests_PROGRAMS = sp68
sp68_SOURCES = init.c

dist_rtems_tests_DATA = sp68.scn
dist_rtems_tests_DATA += sp68.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp68_OBJECTS)
LINK_LIBS = $(sp68_LDLIBS)

sp68$(EXEEXT): $(sp68_OBJECTS) $(sp68_DEPENDENCIES)
	@rm -f sp68$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
