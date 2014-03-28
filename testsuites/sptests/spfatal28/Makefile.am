rtems_tests_PROGRAMS = spfatal28
spfatal28_SOURCES = ../spfatal_support/init.c ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal28.scn spfatal28.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal28_OBJECTS)
LINK_LIBS = $(spfatal28_LDLIBS)

spfatal28$(EXEEXT): $(spfatal28_OBJECTS) $(spfatal28_DEPENDENCIES)
	@rm -f spfatal28$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
