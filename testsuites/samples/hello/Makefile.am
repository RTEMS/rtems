
rtems_tests_PROGRAMS = hello
hello_SOURCES = init.c

dist_rtems_tests_DATA = hello.scn
dist_rtems_tests_DATA += hello.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


LINK_OBJS = $(hello_OBJECTS)
LINK_LIBS = $(hello_LDLIBS)

hello$(EXEEXT): $(hello_OBJECTS) $(hello_DEPENDENCIES)
	@rm -f hello$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
