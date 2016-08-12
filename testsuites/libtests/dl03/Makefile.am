rtems_tests_PROGRAMS = dl03
dl03_SOURCES = init.c dl-cache.c

dist_rtems_tests_DATA = dl03.scn dl03.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(dl03_OBJECTS)
LINK_LIBS = $(dl03_LDLIBS)

dl03$(EXEEXT):  $(dl03_OBJECTS) $(dl03_DEPENDENCIES)
	@rm -f dl03$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
