
rtems_tests_PROGRAMS = sp38
sp38_SOURCES = init.c system.h

dist_rtems_tests_DATA = sp38.scn
dist_rtems_tests_DATA += sp38.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp38_OBJECTS)
LINK_LIBS = $(sp38_LDLIBS)

sp38$(EXEEXT): $(sp38_OBJECTS) $(sp38_DEPENDENCIES)
	@rm -f sp38$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
