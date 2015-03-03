
rtems_tests_PROGRAMS = mdosfs_fsscandir01
mdosfs_fsscandir01_SOURCES  = ../fsscandir01/init.c
mdosfs_fsscandir01_SOURCES += ../support/ramdisk_support.c
mdosfs_fsscandir01_SOURCES += ../support/fstest_support.c
mdosfs_fsscandir01_SOURCES += ../support/fstest_support.h
mdosfs_fsscandir01_SOURCES += ../support/ramdisk_support.h
mdosfs_fsscandir01_SOURCES += ../support/fstest.h
mdosfs_fsscandir01_SOURCES += ../../psxtests/include/pmacros.h
mdosfs_fsscandir01_SOURCES += ../mdosfs_support/fs_support.c
mdosfs_fsscandir01_SOURCES += ../mdosfs_support/fs_config.h

dist_rtems_tests_DATA = mdosfs_fsscandir01.scn
#dist_rtems_tests_DATA += mdosfs_fsscandir01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mdosfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mdosfs_fsscandir01_OBJECTS)
LINK_LIBS = $(mdosfs_fsscandir01_LDLIBS)

mdosfs_fsscandir01$(EXEEXT): $(mdosfs_fsscandir01_OBJECTS) $(mdosfs_fsscandir01_DEPENDENCIES)
	@rm -f mdosfs_fsscandir01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
