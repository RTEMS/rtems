
rtems_tests_PROGRAMS = spnotepad02
spnotepad02_SOURCES = init.c

dist_rtems_tests_DATA = spnotepad02.scn
dist_rtems_tests_DATA += spnotepad02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spnotepad02_OBJECTS)
LINK_LIBS = $(spnotepad02_LDLIBS)

spnotepad02$(EXEEXT): $(spnotepad02_OBJECTS) $(spnotepad02_DEPENDENCIES)
	@rm -f spnotepad02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
