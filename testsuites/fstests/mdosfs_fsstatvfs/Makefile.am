
rtems_tests_PROGRAMS = mdosfs_fsstatvfs
mdosfs_fsstatvfs_SOURCES  = ../fsstatvfs/test.c
mdosfs_fsstatvfs_SOURCES += ../support/ramdisk_support.c
mdosfs_fsstatvfs_SOURCES += ../support/fstest_support.c
mdosfs_fsstatvfs_SOURCES += ../support/fstest_support.h
mdosfs_fsstatvfs_SOURCES += ../support/ramdisk_support.h
mdosfs_fsstatvfs_SOURCES += ../support/fstest.h
mdosfs_fsstatvfs_SOURCES += ../../psxtests/include/pmacros.h
mdosfs_fsstatvfs_SOURCES += ../mdosfs_support/fs_support.c
mdosfs_fsstatvfs_SOURCES += ../mdosfs_support/fs_config.h

dist_rtems_tests_DATA = mdosfs_fsstatvfs.scn
#dist_rtems_tests_DATA = mdosfs_fsstatvfs.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mdosfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mdosfs_fsstatvfs_OBJECTS)
LINK_LIBS = $(mdosfs_fsstatvfs_LDLIBS)

mdosfs_ffsstatvfs$(EXEEXT): $(mdosfs_fsstatvfs_OBJECTS) $(mdosfs_fsstatvfs_DEPENDENCIES)
	@rm -f mdosfs_fsstatvfs$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
