
rtems_tests_PROGRAMS = mrfs_fspermission
mrfs_fspermission_SOURCES  = ../fspermission/test.c
mrfs_fspermission_SOURCES += ../support/ramdisk_support.c
mrfs_fspermission_SOURCES += ../support/fstest_support.c
mrfs_fspermission_SOURCES += ../support/fstest_support.h
mrfs_fspermission_SOURCES += ../support/ramdisk_support.h
mrfs_fspermission_SOURCES += ../support/fstest.h
mrfs_fspermission_SOURCES += ../../psxtests/include/pmacros.h
mrfs_fspermission_SOURCES += ../mrfs_support/fs_support.c
mrfs_fspermission_SOURCES += ../mrfs_support/fs_config.h

#dist_rtems_tests_DATA = mrfs_fspermission.scn
#dist_rtems_tests_DATA += mrfs_fspermission.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mrfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mrfs_fspermission_OBJECTS)
LINK_LIBS = $(mrfs_fspermission_LDLIBS)

mrfs_fspermission$(EXEEXT): $(mrfs_fspermission_OBJECTS) $(mrfs_fspermission_DEPENDENCIES)
	@rm -f mrfs_fspermission$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
