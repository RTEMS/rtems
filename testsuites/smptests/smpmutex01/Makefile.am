rtems_tests_PROGRAMS = smpmutex01
smpmutex01_SOURCES = init.c

dist_rtems_tests_DATA = smpmutex01.scn smpmutex01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpmutex01_OBJECTS)
LINK_LIBS = $(smpmutex01_LDLIBS)

smpmutex01$(EXEEXT): $(smpmutex01_OBJECTS) $(smpmutex01_DEPENDENCIES)
	@rm -f smpmutex01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
