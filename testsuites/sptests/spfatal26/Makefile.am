rtems_tests_PROGRAMS = spfatal26
spfatal26_SOURCES = init.c

dist_rtems_tests_DATA = spfatal26.scn spfatal26.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal26_OBJECTS)
LINK_LIBS = $(spfatal26_LDLIBS)

spfatal26$(EXEEXT): $(spfatal26_OBJECTS) $(spfatal26_DEPENDENCIES)
	@rm -f spfatal26$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
