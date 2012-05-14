
rtems_tests_PROGRAMS = spnsext01
spnsext01_SOURCES = init.c

dist_rtems_tests_DATA = spnsext01.scn spnsext01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spnsext01_OBJECTS)
LINK_LIBS = $(spnsext01_LDLIBS)

spnsext01$(EXEEXT): $(spnsext01_OBJECTS) $(spnsext01_DEPENDENCIES)
	@rm -f spnsext01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
