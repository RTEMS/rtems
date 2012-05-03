
rtems_tests_PROGRAMS = mrfs_fslink
mrfs_fslink_SOURCES  = ../fslink/test.c
mrfs_fslink_SOURCES += ../support/ramdisk_support.c
mrfs_fslink_SOURCES += ../support/fstest_support.c
mrfs_fslink_SOURCES += ../support/fstest_support.h
mrfs_fslink_SOURCES += ../support/ramdisk_support.h
mrfs_fslink_SOURCES += ../support/fstest.h
mrfs_fslink_SOURCES += ../../psxtests/include/pmacros.h
mrfs_fslink_SOURCES += ../mrfs_support/fs_support.c
mrfs_fslink_SOURCES += ../mrfs_support/fs_config.h

#dist_rtems_tests_DATA = mrfs_fslink.scn
#dist_rtems_tests_DATA += mrfs_fslink.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mrfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mrfs_fslink_OBJECTS)
LINK_LIBS = $(mrfs_fslink_LDLIBS)

mrfs_fslink$(EXEEXT): $(mrfs_fslink_OBJECTS) $(mrfs_fslink_DEPENDENCIES)
	@rm -f mrfs_fslink$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
