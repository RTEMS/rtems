
rtems_tests_PROGRAMS = psxmmap01
psxmmap01_SOURCES = init.c test_helper.c test_driver.c system.h

dist_rtems_tests_DATA = psxmmap01.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(psxmmap01_OBJECTS)
LINK_LIBS = $(psxmmap01_LDLIBS)

psxmmap01$(EXEEXT): $(psxmmap01_OBJECTS) $(psxmmap01_DEPENDENCIES)
	@rm -f psxmmap01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
