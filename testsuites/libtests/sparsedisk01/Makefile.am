rtems_tests_PROGRAMS = sparsedisk01
sparsedisk01_SOURCES = init.c

dist_rtems_tests_DATA = sparsedisk01.scn sparsedisk01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sparsedisk01_OBJECTS)
LINK_LIBS = $(sparsedisk01_LDLIBS)

sparsedisk01$(EXEEXT): $(sparsedisk01_OBJECTS) $(sparsedisk01_DEPENDENCIES)
	@rm -f sparsedisk01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
