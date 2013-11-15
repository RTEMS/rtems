
rtems_tests_PROGRAMS = mrfs_fsfpathconf
mrfs_fsfpathconf_SOURCES  = ../fsfpathconf/test.c
mrfs_fsfpathconf_SOURCES += ../support/ramdisk_support.c
mrfs_fsfpathconf_SOURCES += ../support/fstest_support.c
mrfs_fsfpathconf_SOURCES += ../support/fstest_support.h
mrfs_fsfpathconf_SOURCES += ../support/ramdisk_support.h
mrfs_fsfpathconf_SOURCES += ../support/fstest.h
mrfs_fsfpathconf_SOURCES += ../../psxtests/include/pmacros.h
mrfs_fsfpathconf_SOURCES += ../mrfs_support/fs_support.c
mrfs_fsfpathconf_SOURCES += ../mrfs_support/fs_config.h

dist_rtems_tests_DATA = mrfs_fsfpathconf.scn
#dist_rtems_tests_DATA += mrfs_fsfpathconf.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mrfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mrfs_fsfpathconf_OBJECTS)
LINK_LIBS = $(mrfs_fsfpathconf_LDLIBS)

mrfs_fsfpathconf$(EXEEXT): $(mrfs_fsfpathconf_OBJECTS) $(mrfs_fsfpathconf_DEPENDENCIES)
	@rm -f mrfs_fsfpathconf$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
