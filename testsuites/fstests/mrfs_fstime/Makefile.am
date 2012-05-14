
rtems_tests_PROGRAMS = mrfs_fstime
mrfs_fstime_SOURCES  = ../fstime/test.c
mrfs_fstime_SOURCES += ../support/ramdisk_support.c
mrfs_fstime_SOURCES += ../support/fstest_support.c
mrfs_fstime_SOURCES += ../support/fstest_support.h
mrfs_fstime_SOURCES += ../support/ramdisk_support.h
mrfs_fstime_SOURCES += ../support/fstest.h
mrfs_fstime_SOURCES += ../../psxtests/include/pmacros.h
mrfs_fstime_SOURCES += ../mrfs_support/fs_support.c
mrfs_fstime_SOURCES += ../mrfs_support/fs_config.h

#dist_rtems_tests_DATA = mrfs_fstime.scn
#dist_rtems_tests_DATA += mrfs_fstime.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mrfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mrfs_fstime_OBJECTS)
LINK_LIBS = $(mrfs_fstime_LDLIBS)

mrfs_fstime$(EXEEXT): $(mrfs_fstime_OBJECTS) $(mrfs_fstime_DEPENDENCIES)
	@rm -f mrfs_fstime$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
