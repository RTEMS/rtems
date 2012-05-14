
rtems_tests_PROGRAMS = sp62
sp62_SOURCES = init.c

dist_rtems_tests_DATA = sp62.scn
dist_rtems_tests_DATA += sp62.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(sp62_OBJECTS)
LINK_LIBS = $(sp62_LDLIBS)

sp62$(EXEEXT): $(sp62_OBJECTS) $(sp62_DEPENDENCIES)
	@rm -f sp62$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
