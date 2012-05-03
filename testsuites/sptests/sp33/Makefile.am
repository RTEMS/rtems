
rtems_tests_PROGRAMS = sp33
sp33_SOURCES = init.c

dist_rtems_tests_DATA = sp33.scn
dist_rtems_tests_DATA += sp33.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp33_OBJECTS)
LINK_LIBS = $(sp33_LDLIBS)

sp33$(EXEEXT): $(sp33_OBJECTS) $(sp33_DEPENDENCIES)
	@rm -f sp33$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
