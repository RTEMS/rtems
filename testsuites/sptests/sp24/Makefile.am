
rtems_tests_PROGRAMS = sp24
sp24_SOURCES = init.c resume.c task1.c system.h

dist_rtems_tests_DATA = sp24.scn
dist_rtems_tests_DATA += sp24.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp24_OBJECTS)
LINK_LIBS = $(sp24_LDLIBS)

sp24$(EXEEXT): $(sp24_OBJECTS) $(sp24_DEPENDENCIES)
	@rm -f sp24$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
