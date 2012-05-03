
rtems_tests_PROGRAMS = sp51
sp51_SOURCES = init.c

dist_rtems_tests_DATA = sp51.scn
dist_rtems_tests_DATA += sp51.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp51_OBJECTS)
LINK_LIBS = $(sp51_LDLIBS)

sp51$(EXEEXT): $(sp51_OBJECTS) $(sp51_DEPENDENCIES)
	@rm -f sp51$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
