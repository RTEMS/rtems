
rtems_tests_PROGRAMS = malloctest
malloctest_SOURCES = init.c task1.c system.h

dist_rtems_tests_DATA = malloctest.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(malloctest_OBJECTS)
LINK_LIBS = $(malloctest_LDLIBS)

malloctest$(EXEEXT): $(malloctest_OBJECTS) $(malloctest_DEPENDENCIES)
	@rm -f malloctest$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
