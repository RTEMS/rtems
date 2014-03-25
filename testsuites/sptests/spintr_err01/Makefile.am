
rtems_tests_PROGRAMS = spintr_err01
spintr_err01_SOURCES = init.c isr.c system.h

dist_rtems_tests_DATA = spintr_err01.scn
dist_rtems_tests_DATA += spintr_err01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spintr_err01_OBJECTS)
LINK_LIBS = $(spintr_err01_LDLIBS)

spintr_err01$(EXEEXT): $(spintr_err01_OBJECTS) $(spintr_err01_DEPENDENCIES)
	@rm -f spintr_err01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
