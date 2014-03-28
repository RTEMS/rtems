rtems_tests_PROGRAMS = spthreadlife01
spthreadlife01_SOURCES = init.c

dist_rtems_tests_DATA = spthreadlife01.scn spthreadlife01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spthreadlife01_OBJECTS)
LINK_LIBS = $(spthreadlife01_LDLIBS)

spthreadlife01$(EXEEXT): $(spthreadlife01_OBJECTS) $(spthreadlife01_DEPENDENCIES)
	@rm -f spthreadlife01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
