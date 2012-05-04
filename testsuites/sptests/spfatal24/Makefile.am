rtems_tests_PROGRAMS = spfatal24
spfatal24_SOURCES = ../spfatal_support/init.c \
    ../spfatal_support/system.h testcase.h

dist_rtems_tests_DATA = spfatal24.scn
dist_rtems_tests_DATA += spfatal24.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfatal24_OBJECTS)
LINK_LIBS = $(spfatal24_LDLIBS)

spfatal24$(EXEEXT): $(spfatal24_OBJECTS) $(spfatal24_DEPENDENCIES)
	@rm -f spfatal24$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
