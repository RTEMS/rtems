
rtems_tests_PROGRAMS = spstkalloc
spstkalloc_SOURCES = init.c

dist_rtems_tests_DATA = spstkalloc.scn
dist_rtems_tests_DATA += spstkalloc.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spstkalloc_OBJECTS)
LINK_LIBS = $(spstkalloc_LDLIBS)

spstkalloc$(EXEEXT): $(spstkalloc_OBJECTS) $(spstkalloc_DEPENDENCIES)
	@rm -f spstkalloc$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
