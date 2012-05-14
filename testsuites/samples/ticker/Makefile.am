
rtems_tests_PROGRAMS = ticker
ticker_SOURCES = init.c tasks.c system.h

dist_rtems_tests_DATA = ticker.scn
dist_rtems_tests_DATA += ticker.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(ticker_OBJECTS)
LINK_LIBS = $(ticker_LDLIBS)

ticker$(EXEEXT): $(ticker_OBJECTS) $(ticker_DEPENDENCIES)
	@rm -f ticker$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
