rtems_tests_PROGRAMS = fsfseeko01
fsfseeko01_SOURCES = init.c

dist_rtems_tests_DATA = fsfseeko01.scn fsfseeko01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(fsfseeko01_OBJECTS)
LINK_LIBS = $(fsfseeko01_LDLIBS)

fsfseeko01$(EXEEXT): $(fsfseeko01_OBJECTS) $(fsfseeko01_DEPENDENCIES)
	@rm -f fsfseeko01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
