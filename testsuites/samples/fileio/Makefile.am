rtems_tests_PROGRAMS = fileio
fileio_SOURCES = init.c system.h

dist_rtems_tests_DATA = fileio.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fileio_OBJECTS)
LINK_LIBS = $(fileio_LDLIBS)

fileio$(EXEEXT): $(fileio_OBJECTS) $(fileio_DEPENDENCIES)
	@rm -f fileio$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
