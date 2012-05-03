
rtems_tests_PROGRAMS = mimfs_fstime
mimfs_fstime_SOURCES  = ../fstime/test.c
mimfs_fstime_SOURCES += ../support/fstest_support.c
mimfs_fstime_SOURCES += ../support/fstest_support.h
mimfs_fstime_SOURCES += ../support/fstest.h
mimfs_fstime_SOURCES += ../../psxtests/include/pmacros.h
mimfs_fstime_SOURCES += ../mimfs_support/fs_support.c
mimfs_fstime_SOURCES += ../mimfs_support/fs_config.h

#dist_rtems_tests_DATA = mimfs_fstime.scn
#dist_rtems_tests_DATA += mimfs_fstime.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mimfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mimfs_fstime_OBJECTS)
LINK_LIBS = $(mimfs_fstime_LDLIBS)

mimfs_fstime$(EXEEXT): $(mimfs_fstime_OBJECTS) $(mimfs_fstime_DEPENDENCIES)
	@rm -f mimfs_fstime$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
