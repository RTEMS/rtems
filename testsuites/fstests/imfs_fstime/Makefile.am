
rtems_tests_PROGRAMS = imfs_fstime
imfs_fstime_SOURCES  = ../fstime/test.c
imfs_fstime_SOURCES += ../support/fstest_support.c
imfs_fstime_SOURCES += ../support/fstest_support.h
imfs_fstime_SOURCES += ../support/fstest.h
imfs_fstime_SOURCES += ../../psxtests/include/pmacros.h
imfs_fstime_SOURCES += ../imfs_support/fs_support.c
imfs_fstime_SOURCES += ../imfs_support/fs_config.h

#dist_rtems_tests_DATA = imfs_fstime.scn
#dist_rtems_tests_DATA += imfs_fstime.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/imfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(imfs_fstime_OBJECTS)
LINK_LIBS = $(imfs_fstime_LDLIBS)

imfs_fstime$(EXEEXT): $(imfs_fstime_OBJECTS) $(imfs_fstime_DEPENDENCIES)
	@rm -f imfs_fstime$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
