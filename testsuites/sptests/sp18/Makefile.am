
rtems_tests_PROGRAMS = sp18
sp18_SOURCES = init.c

dist_rtems_tests_DATA = sp18.scn
dist_rtems_tests_DATA += sp18.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp18_OBJECTS)
LINK_LIBS = $(sp18_LDLIBS)

sp18$(EXEEXT): $(sp18_OBJECTS) $(sp18_DEPENDENCIES)
	@rm -f sp18$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
