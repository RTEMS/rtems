
rtems_tests_PROGRAMS = base_sp
base_sp_SOURCES = init.c apptask.c system.h

dist_rtems_tests_DATA = base_sp.scn
dist_rtems_tests_DATA += base_sp.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include


LINK_OBJS = $(base_sp_OBJECTS)
LINK_LIBS = $(base_sp_LDLIBS)

base_sp$(EXEEXT): $(base_sp_OBJECTS) $(base_sp_DEPENDENCIES)
	@rm -f base_sp$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
