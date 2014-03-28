
rtems_tests_PROGRAMS = spclock_err01
spclock_err01_SOURCES = init.c delay.c system.h

dist_rtems_tests_DATA = spclock_err01.scn
dist_rtems_tests_DATA += spclock_err01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spclock_err01_OBJECTS)
LINK_LIBS = $(spclock_err01_LDLIBS)

spclock_err01$(EXEEXT): $(spclock_err01_OBJECTS) $(spclock_err01_DEPENDENCIES)
	@rm -f spclock_err01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
