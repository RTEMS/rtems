
rtems_tests_PROGRAMS = cxx_iostream
cxx_iostream_SOURCES = init.cc system.h

dist_rtems_tests_DATA = iostream.scn
dist_rtems_tests_DATA += iostream.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


LINK_OBJS = $(cxx_iostream_OBJECTS)
LINK_LIBS = $(cxx_iostream_LDLIBS)

cxx_iostream$(EXEEXT): $(cxx_iostream_OBJECTS) $(cxx_iostream_DEPENDENCIES)
	@rm -f cxx_iostream$(EXEEXT)
	$(make-cxx-exe)

include $(top_srcdir)/../automake/local.am
