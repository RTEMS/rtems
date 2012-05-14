
rtems_tests_PROGRAMS = sp07
sp07_SOURCES = init.c buffered_io.c task1.c task2.c task3.c task4.c \
    taskexit.c tcreate.c tdelete.c trestart.c tstart.c system.h

dist_rtems_tests_DATA = sp07.scn
dist_rtems_tests_DATA += sp07.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp07_OBJECTS)
LINK_LIBS = $(sp07_LDLIBS)

sp07$(EXEEXT): $(sp07_OBJECTS) $(sp07_DEPENDENCIES)
	@rm -f sp07$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
