rtems_tests_PROGRAMS = fsdosfsname01
fsdosfsname01_SOURCES = init.c
fsdosfsname01_SOURCES += ../support/ramdisk_support.c

dist_rtems_tests_DATA = fsdosfsname01.scn fsdosfsname01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include 
AM_CPPFLAGS += -I$(top_srcdir)/support

LINK_OBJS = $(fsdosfsname01_OBJECTS)
LINK_LIBS = $(fsdosfsname01_LDLIBS)

fsdosfsname01$(EXEEXT): $(fsdosfsname01_OBJECTS) $(fsdosfsname01_DEPENDENCIES)
	@rm -f fsdosfsname01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
