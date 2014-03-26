
rtems_tests_PROGRAMS = spsem_err01
spsem_err01_SOURCES = init.c system.h

dist_rtems_tests_DATA = spsem_err01.scn
dist_rtems_tests_DATA += spsem_err01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spsem_err01_OBJECTS)
LINK_LIBS = $(spsem_err01_LDLIBS)

spsem_err01$(EXEEXT): $(spsem_err01_OBJECTS) $(spsem_err01_DEPENDENCIES)
	@rm -f spsem_err01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
