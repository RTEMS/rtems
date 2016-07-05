rtems_tests_PROGRAMS = smpmutex02
smpmutex02_SOURCES = init.c

dist_rtems_tests_DATA = smpmutex02.scn smpmutex02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(smpmutex02_OBJECTS)
LINK_LIBS = $(smpmutex02_LDLIBS)

smpmutex02$(EXEEXT): $(smpmutex02_OBJECTS) $(smpmutex02_DEPENDENCIES)
	@rm -f smpmutex02$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
