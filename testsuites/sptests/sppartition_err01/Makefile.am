
rtems_tests_PROGRAMS = sppartition_err01
sppartition_err01_SOURCES = init.c system.h

dist_rtems_tests_DATA = sppartition_err01.scn
dist_rtems_tests_DATA += sppartition_err01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sppartition_err01_OBJECTS)
LINK_LIBS = $(sppartition_err01_LDLIBS)

sppartition_err01$(EXEEXT): $(sppartition_err01_OBJECTS) $(sppartition_err01_DEPENDENCIES)
	@rm -f sppartition_err01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
