
rtems_tests_PROGRAMS = imfs_fsscandir01
imfs_fsscandir01_SOURCES  = ../fsscandir01/init.c
imfs_fsscandir01_SOURCES += ../support/fstest_support.c
imfs_fsscandir01_SOURCES += ../support/fstest_support.h
imfs_fsscandir01_SOURCES += ../support/fstest.h
imfs_fsscandir01_SOURCES += ../../psxtests/include/pmacros.h
imfs_fsscandir01_SOURCES += ../imfs_support/fs_support.c
imfs_fsscandir01_SOURCES += ../imfs_support/fs_config.h 

#dist_rtems_tests_DATA = imfs_fsscandir01.scn
#dist_rtems_tests_DATA += imfs_fsscandir01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/imfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(imfs_fsscandir01_OBJECTS)
LINK_LIBS = $(imfs_fsscandir01_LDLIBS)

imfs_fsscandir01$(EXEEXT): $(imfs_fsscandir01_OBJECTS) $(imfs_fsscandir01_DEPENDENCIES)
	@rm -f imfs_fsscandir01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
