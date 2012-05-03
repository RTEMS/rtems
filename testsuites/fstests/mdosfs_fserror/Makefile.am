
rtems_tests_PROGRAMS = mdosfs_fserror
mdosfs_fserror_SOURCES  = ../fserror/test.c
mdosfs_fserror_SOURCES += ../support/ramdisk_support.c
mdosfs_fserror_SOURCES += ../support/fstest_support.c
mdosfs_fserror_SOURCES += ../support/fstest_support.h
mdosfs_fserror_SOURCES += ../support/ramdisk_support.h
mdosfs_fserror_SOURCES += ../support/fstest.h
mdosfs_fserror_SOURCES += ../../psxtests/include/pmacros.h
mdosfs_fserror_SOURCES += ../mdosfs_support/fs_support.c
mdosfs_fserror_SOURCES += ../mdosfs_support/fs_config.h

#dist_rtems_tests_DATA = mdosfs_fserror.scn
#dist_rtems_tests_DATA += mdosfs_fserror.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mdosfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mdosfs_fserror_OBJECTS)
LINK_LIBS = $(mdosfs_fserror_LDLIBS)

mdosfs_fserror$(EXEEXT): $(mdosfs_fserror_OBJECTS) $(mdosfs_fserror_DEPENDENCIES)
	@rm -f mdosfs_fserror$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
