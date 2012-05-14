
rtems_tests_PROGRAMS = spcbssched03
spcbssched03_SOURCES = init.c tasks_periodic.c tasks_aperiodic.c system.h \
    cbsparams.h

dist_rtems_tests_DATA = spcbssched03.scn
dist_rtems_tests_DATA += spcbssched03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spcbssched03_OBJECTS)
LINK_LIBS = $(spcbssched03_LDLIBS)

spcbssched03$(EXEEXT): $(spcbssched03_OBJECTS) $(spcbssched03_DEPENDENCIES)
	@rm -f spcbssched03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
