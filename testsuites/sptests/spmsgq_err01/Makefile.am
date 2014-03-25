
rtems_tests_PROGRAMS = spmsgq_err01
spmsgq_err01_SOURCES = init.c system.h

dist_rtems_tests_DATA = spmsgq_err01.scn
dist_rtems_tests_DATA += spmsgq_err01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spmsgq_err01_OBJECTS)
LINK_LIBS = $(spmsgq_err01_LDLIBS)

spmsgq_err01$(EXEEXT): $(spmsgq_err01_OBJECTS) $(spmsgq_err01_DEPENDENCIES)
	@rm -f spmsgq_err01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
