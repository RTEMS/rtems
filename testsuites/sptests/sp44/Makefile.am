
rtems_tests_PROGRAMS = sp44
sp44_SOURCES = init.c

dist_rtems_tests_DATA = sp44.scn
dist_rtems_tests_DATA += sp44.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp44_OBJECTS)
LINK_LIBS = $(sp44_LDLIBS)

sp44$(EXEEXT): $(sp44_OBJECTS) $(sp44_DEPENDENCIES)
	@rm -f sp44$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
