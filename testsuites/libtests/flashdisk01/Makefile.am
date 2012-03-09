rtems_tests_PROGRAMS = flashdisk01
flashdisk01_SOURCES = init.c test-file-system.c

dist_rtems_tests_DATA = flashdisk01.scn flashdisk01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(flashdisk01_OBJECTS)
LINK_LIBS = $(flashdisk01_LDLIBS)

flashdisk01$(EXEEXT): $(flashdisk01_OBJECTS) $(flashdisk01_DEPENDENCIES)
	@rm -f flashdisk01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
