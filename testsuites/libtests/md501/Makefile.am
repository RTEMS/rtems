rtems_tests_PROGRAMS = md501
md501_SOURCES = init.c

dist_rtems_tests_DATA = md501.scn md501.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(md501_OBJECTS)
LINK_LIBS = $(md501_LDLIBS)

md501$(EXEEXT): $(md501_OBJECTS) $(md501_DEPENDENCIES)
	@rm -f md501$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
