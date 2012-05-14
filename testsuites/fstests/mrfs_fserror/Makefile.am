
rtems_tests_PROGRAMS = mrfs_fserror
mrfs_fserror_SOURCES  = ../fserror/test.c
mrfs_fserror_SOURCES += ../support/ramdisk_support.c
mrfs_fserror_SOURCES += ../support/fstest_support.c
mrfs_fserror_SOURCES += ../support/fstest_support.h
mrfs_fserror_SOURCES += ../support/ramdisk_support.h
mrfs_fserror_SOURCES += ../support/fstest.h
mrfs_fserror_SOURCES += ../../psxtests/include/pmacros.h
mrfs_fserror_SOURCES += ../mrfs_support/fs_support.c
mrfs_fserror_SOURCES += ../mrfs_support/fs_config.h

#dist_rtems_tests_DATA = mrfs_fserror.scn
#dist_rtems_tests_DATA += mrfs_fserror.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mrfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mrfs_fserror_OBJECTS)
LINK_LIBS = $(mrfs_fserror_LDLIBS)

mrfs_fserror$(EXEEXT): $(mrfs_fserror_OBJECTS) $(mrfs_fserror_DEPENDENCIES)
	@rm -f mrfs_fserror$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
