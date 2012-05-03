
rtems_tests_PROGRAMS = cdtest
cdtest_SOURCES = init.c main.cc system.h

dist_rtems_tests_DATA = cdtest.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


# AM_CPPFLAGS += -DRTEMS_TEST_IO_STREAM

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(cdtest_OBJECTS)
LINK_LIBS = $(cdtest_LDLIBS)

cdtest$(EXEEXT): $(cdtest_OBJECTS) $(cdtest_DEPENDENCIES)
	@rm -f cdtest$(EXEEXT)
	$(make-cxx-exe)

include $(top_srcdir)/../automake/local.am
