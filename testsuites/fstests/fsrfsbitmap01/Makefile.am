
rtems_tests_PROGRAMS = fsrfsbitmap01
fsrfsbitmap01_SOURCES  = test.c
fsrfsbitmap01_SOURCES += ../support/ramdisk_support.c
fsrfsbitmap01_SOURCES += ../support/fstest_support.c
fsrfsbitmap01_SOURCES += ../support/fstest_support.h
fsrfsbitmap01_SOURCES += ../support/ramdisk_support.h
fsrfsbitmap01_SOURCES += ../support/fstest.h
fsrfsbitmap01_SOURCES += ../../psxtests/include/pmacros.h
fsrfsbitmap01_SOURCES += ../mrfs_support/fs_support.c
fsrfsbitmap01_SOURCES += ../mrfs_support/fs_config.h

dist_rtems_tests_DATA = fsrfsbitmap01.scn
dist_rtems_tests_DATA += fsrfsbitmap01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mrfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(fsrfsbitmap01_OBJECTS)
LINK_LIBS = $(fsrfsbitmap01_LDLIBS)

fsrfsbitmap01$(EXEEXT): $(fsrfsbitmap01_OBJECTS) $(fsrfsbitmap01_DEPENDENCIES)
	@rm -f fsrfsbitmap01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
