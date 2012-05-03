
rtems_tests_PROGRAMS = mimfs_fslink
mimfs_fslink_SOURCES  = ../fslink/test.c
mimfs_fslink_SOURCES += ../support/fstest_support.c
mimfs_fslink_SOURCES += ../support/fstest_support.h
mimfs_fslink_SOURCES += ../support/fstest.h
mimfs_fslink_SOURCES += ../../psxtests/include/pmacros.h
mimfs_fslink_SOURCES += ../mimfs_support/fs_support.c
mimfs_fslink_SOURCES += ../mimfs_support/fs_config.h

#dist_rtems_tests_DATA = mimfs_fslink.scn
#dist_rtems_tests_DATA += mimfs_fslink.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mimfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mimfs_fslink_OBJECTS)
LINK_LIBS = $(mimfs_fslink_LDLIBS)

mimfs_fslink$(EXEEXT): $(mimfs_fslink_OBJECTS) $(mimfs_fslink_DEPENDENCIES)
	@rm -f mimfs_fslink$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
