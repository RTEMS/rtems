
rtems_tests_PROGRAMS = mdosfs_fstime
mdosfs_fstime_SOURCES  = ../fstime/test.c
mdosfs_fstime_SOURCES += ../support/ramdisk_support.c
mdosfs_fstime_SOURCES += ../support/fstest_support.c
mdosfs_fstime_SOURCES += ../support/fstest_support.h
mdosfs_fstime_SOURCES += ../support/ramdisk_support.h
mdosfs_fstime_SOURCES += ../support/fstest.h
mdosfs_fstime_SOURCES += ../../psxtests/include/pmacros.h
mdosfs_fstime_SOURCES += ../mdosfs_support/fs_support.c
mdosfs_fstime_SOURCES += ../mdosfs_support/fs_config.h

#dist_rtems_tests_DATA = mdosfs_fstime.scn
#dist_rtems_tests_DATA += mdosfs_fstime.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mdosfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mdosfs_fstime_OBJECTS)
LINK_LIBS = $(mdosfs_fstime_LDLIBS)

mdosfs_fstime$(EXEEXT): $(mdosfs_fstime_OBJECTS) $(mdosfs_fstime_DEPENDENCIES)
	@rm -f mdosfs_fstime$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
