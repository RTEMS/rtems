
rtems_tests_PROGRAMS = mdosfs_fsrdwr
mdosfs_fsrdwr_SOURCES  = ../fsrdwr/init.c
mdosfs_fsrdwr_SOURCES += ../support/ramdisk_support.c
mdosfs_fsrdwr_SOURCES += ../support/fstest_support.c
mdosfs_fsrdwr_SOURCES += ../support/fstest_support.h
mdosfs_fsrdwr_SOURCES += ../support/ramdisk_support.h
mdosfs_fsrdwr_SOURCES += ../support/fstest.h
mdosfs_fsrdwr_SOURCES += ../../psxtests/include/pmacros.h
mdosfs_fsrdwr_SOURCES += ../mdosfs_support/fs_support.c
mdosfs_fsrdwr_SOURCES += ../mdosfs_support/fs_config.h

#dist_rtems_tests_DATA = mdosfs_fsrdwr.scn
#dist_rtems_tests_DATA += mdosfs_fsrdwr.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mdosfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mdosfs_fsrdwr_OBJECTS)
LINK_LIBS = $(mdosfs_fsrdwr_LDLIBS)

mdosfs_fsrdwr$(EXEEXT): $(mdosfs_fsrdwr_OBJECTS) $(mdosfs_fsrdwr_DEPENDENCIES)
	@rm -f mdosfs_fsrdwr$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
