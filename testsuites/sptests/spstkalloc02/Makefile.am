
rtems_tests_PROGRAMS = spstkalloc02
spstkalloc02_SOURCES = init.c

dist_rtems_tests_DATA = spstkalloc02.scn
dist_rtems_tests_DATA += spstkalloc02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spstkalloc02_OBJECTS)
LINK_LIBS = $(spstkalloc02_LDLIBS)

spstkalloc02$(EXEEXT): $(spstkalloc02_OBJECTS) $(spstkalloc02_DEPENDENCIES)
	@rm -f spstkalloc02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
