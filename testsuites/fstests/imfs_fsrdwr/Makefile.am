
rtems_tests_PROGRAMS = imfs_fsrdwr
imfs_fsrdwr_SOURCES  = ../fsrdwr/init.c
imfs_fsrdwr_SOURCES += ../support/fstest_support.c
imfs_fsrdwr_SOURCES += ../support/fstest_support.h
imfs_fsrdwr_SOURCES += ../support/fstest.h
imfs_fsrdwr_SOURCES += ../../psxtests/include/pmacros.h
imfs_fsrdwr_SOURCES += ../imfs_support/fs_support.c
imfs_fsrdwr_SOURCES += ../imfs_support/fs_config.h 

#dist_rtems_tests_DATA = imfs_fsrdwr.scn
#dist_rtems_tests_DATA += imfs_fsrdwr.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/imfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(imfs_fsrdwr_OBJECTS)
LINK_LIBS = $(imfs_fsrdwr_LDLIBS)

imfs_fsrdwr$(EXEEXT): $(imfs_fsrdwr_OBJECTS) $(imfs_fsrdwr_DEPENDENCIES)
	@rm -f imfs_fsrdwr$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
