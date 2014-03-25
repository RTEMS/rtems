
rtems_tests_PROGRAMS = spsignal_err01
spsignal_err01_SOURCES = init.c isr.c system.h

dist_rtems_tests_DATA = spsignal_err01.scn
dist_rtems_tests_DATA += spsignal_err01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsignal_err01_OBJECTS)
LINK_LIBS = $(spsignal_err01_LDLIBS)

spsignal_err01$(EXEEXT): $(spsignal_err01_OBJECTS) $(spsignal_err01_DEPENDENCIES)
	@rm -f spsignal_err01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
