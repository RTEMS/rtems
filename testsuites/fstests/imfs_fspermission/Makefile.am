
rtems_tests_PROGRAMS = imfs_fspermission
imfs_fspermission_SOURCES  = ../fspermission/test.c
imfs_fspermission_SOURCES += ../support/fstest_support.c
imfs_fspermission_SOURCES += ../support/fstest_support.h
imfs_fspermission_SOURCES += ../support/fstest.h
imfs_fspermission_SOURCES += ../../psxtests/include/pmacros.h
imfs_fspermission_SOURCES += ../imfs_support/fs_support.c
imfs_fspermission_SOURCES += ../imfs_support/fs_config.h 

#dist_rtems_tests_DATA = imfs_fspermission.scn
#dist_rtems_tests_DATA += imfs_fspermission.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/imfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(imfs_fspermission_OBJECTS)
LINK_LIBS = $(imfs_fspermission_LDLIBS)

imfs_fspermission$(EXEEXT): $(imfs_fspermission_OBJECTS) $(imfs_fspermission_DEPENDENCIES)
	@rm -f imfs_fspermission$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
