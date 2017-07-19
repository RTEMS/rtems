rtems_tests_PROGRAMS = spfatal31
spfatal31_SOURCES = init.c

dist_rtems_tests_DATA = spfatal31.scn spfatal31.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal31_OBJECTS)
LINK_LIBS = $(spfatal31_LDLIBS)

spfatal31$(EXEEXT): $(spfatal31_OBJECTS) $(spfatal31_DEPENDENCIES)
	@rm -f spfatal31$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
