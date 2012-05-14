
rtems_tests_PROGRAMS = spnotepad01
spnotepad01_SOURCES = init.c

dist_rtems_tests_DATA = spnotepad01.scn
dist_rtems_tests_DATA += spnotepad01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spnotepad01_OBJECTS)
LINK_LIBS = $(spnotepad01_LDLIBS)

spnotepad01$(EXEEXT): $(spnotepad01_OBJECTS) $(spnotepad01_DEPENDENCIES)
	@rm -f spnotepad01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
