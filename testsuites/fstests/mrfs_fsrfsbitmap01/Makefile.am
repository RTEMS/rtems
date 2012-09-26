
rtems_tests_PROGRAMS = mrfs_fsrfsbitmap01
mrfs_fsrfsbitmap01_SOURCES  = ../fsrfsbitmap01/test.c
mrfs_fsrfsbitmap01_SOURCES += ../support/ramdisk_support.c
mrfs_fsrfsbitmap01_SOURCES += ../support/fstest_support.c
mrfs_fsrfsbitmap01_SOURCES += ../support/fstest_support.h
mrfs_fsrfsbitmap01_SOURCES += ../support/ramdisk_support.h
mrfs_fsrfsbitmap01_SOURCES += ../support/fstest.h
mrfs_fsrfsbitmap01_SOURCES += ../../psxtests/include/pmacros.h
mrfs_fsrfsbitmap01_SOURCES += ../mrfs_support/fs_support.c
mrfs_fsrfsbitmap01_SOURCES += ../mrfs_support/fs_config.h

#dist_rtems_tests_DATA = mrfs_fsrfsbitmap01.scn
#dist_rtems_tests_DATA += mrfs_fsrfsbitmap01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mrfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mrfs_fsrfsbitmap01_OBJECTS)
LINK_LIBS = $(mrfs_fsrfsbitmap01_LDLIBS)

mrfs_fsrfsbitmap01$(EXEEXT): $(mrfs_fsrfsbitmap01_OBJECTS) $(mrfs_fsrfsbitmap01_DEPENDENCIES)
	@rm -f mrfs_fsrfsbitmap01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
