
rtems_tests_PROGRAMS = mimfs_fsrdwr
mimfs_fsrdwr_SOURCES  = ../fsrdwr/init.c
mimfs_fsrdwr_SOURCES += ../support/fstest_support.c
mimfs_fsrdwr_SOURCES += ../support/fstest_support.h
mimfs_fsrdwr_SOURCES += ../support/fstest.h
mimfs_fsrdwr_SOURCES += ../../psxtests/include/pmacros.h
mimfs_fsrdwr_SOURCES += ../mimfs_support/fs_support.c
mimfs_fsrdwr_SOURCES += ../mimfs_support/fs_config.h

#dist_rtems_tests_DATA = mimfs_fsrdwr.scn
#dist_rtems_tests_DATA += mimfs_fsrdwr.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mimfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mimfs_fsrdwr_OBJECTS)
LINK_LIBS = $(mimfs_fsrdwr_LDLIBS)

mimfs_fsrdwr$(EXEEXT): $(mimfs_fsrdwr_OBJECTS) $(mimfs_fsrdwr_DEPENDENCIES)
	@rm -f mimfs_fsrdwr$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
