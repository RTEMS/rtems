
rtems_tests_PROGRAMS = mrfs_fsscandir01
mrfs_fsscandir01_SOURCES  = ../fsscandir01/init.c
mrfs_fsscandir01_SOURCES += ../support/ramdisk_support.c
mrfs_fsscandir01_SOURCES += ../support/fstest_support.c
mrfs_fsscandir01_SOURCES += ../support/fstest_support.h
mrfs_fsscandir01_SOURCES += ../support/ramdisk_support.h
mrfs_fsscandir01_SOURCES += ../support/fstest.h
mrfs_fsscandir01_SOURCES += ../../psxtests/include/pmacros.h
mrfs_fsscandir01_SOURCES += ../mrfs_support/fs_support.c
mrfs_fsscandir01_SOURCES += ../mrfs_support/fs_config.h

dist_rtems_tests_DATA = mrfs_fsscandir01.scn
#dist_rtems_tests_DATA += mrfs_fsscandir01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mrfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mrfs_fsscandir01_OBJECTS)
LINK_LIBS = $(mrfs_fsscandir01_LDLIBS)

mrfs_fsscandir01$(EXEEXT): $(mrfs_fsscandir01_OBJECTS) $(mrfs_fsscandir01_DEPENDENCIES)
	@rm -f mrfs_fsscandir01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
