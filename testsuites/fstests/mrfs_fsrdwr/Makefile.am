
rtems_tests_PROGRAMS = mrfs_fsrdwr
mrfs_fsrdwr_SOURCES  = ../fsrdwr/init.c
mrfs_fsrdwr_SOURCES += ../support/ramdisk_support.c
mrfs_fsrdwr_SOURCES += ../support/fstest_support.c
mrfs_fsrdwr_SOURCES += ../support/fstest_support.h
mrfs_fsrdwr_SOURCES += ../support/ramdisk_support.h
mrfs_fsrdwr_SOURCES += ../support/fstest.h
mrfs_fsrdwr_SOURCES += ../../psxtests/include/pmacros.h
mrfs_fsrdwr_SOURCES += ../mrfs_support/fs_support.c
mrfs_fsrdwr_SOURCES += ../mrfs_support/fs_config.h

#dist_rtems_tests_DATA = mrfs_fsrdwr.scn
#dist_rtems_tests_DATA += mrfs_fsrdwr.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mrfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mrfs_fsrdwr_OBJECTS)
LINK_LIBS = $(mrfs_fsrdwr_LDLIBS)

mrfs_fsrdwr$(EXEEXT): $(mrfs_fsrdwr_OBJECTS) $(mrfs_fsrdwr_DEPENDENCIES)
	@rm -f mrfs_fsrdwr$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
