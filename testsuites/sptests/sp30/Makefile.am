
rtems_tests_PROGRAMS = sp30
sp30_SOURCES = init.c resume.c task1.c system.h

dist_rtems_tests_DATA = sp30.scn
dist_rtems_tests_DATA += sp30.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp30_OBJECTS)
LINK_LIBS = $(sp30_LDLIBS)

sp30$(EXEEXT): $(sp30_OBJECTS) $(sp30_DEPENDENCIES)
	@rm -f sp30$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
