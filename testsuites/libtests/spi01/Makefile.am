rtems_tests_PROGRAMS = spi01
spi01_SOURCES = init.c

dist_rtems_tests_DATA = spi01.scn spi01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spi01_OBJECTS)
LINK_LIBS = $(spi01_LDLIBS)

spi01$(EXEEXT): $(spi01_OBJECTS) $(spi01_DEPENDENCIES)
	@rm -f spi01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
