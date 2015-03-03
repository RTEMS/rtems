
rtems_tests_PROGRAMS = mimfs_fsscandir01
mimfs_fsscandir01_SOURCES  = ../fsscandir01/init.c
mimfs_fsscandir01_SOURCES += ../support/fstest_support.c
mimfs_fsscandir01_SOURCES += ../support/fstest_support.h
mimfs_fsscandir01_SOURCES += ../support/fstest.h
mimfs_fsscandir01_SOURCES += ../../psxtests/include/pmacros.h
mimfs_fsscandir01_SOURCES += ../mimfs_support/fs_support.c
mimfs_fsscandir01_SOURCES += ../mimfs_support/fs_config.h

#dist_rtems_tests_DATA = mimfs_fsscandir01.scn
#dist_rtems_tests_DATA += mimfs_fsscandir01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mimfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mimfs_fsscandir01_OBJECTS)
LINK_LIBS = $(mimfs_fsscandir01_LDLIBS)

mimfs_fsscandir01$(EXEEXT): $(mimfs_fsscandir01_OBJECTS) $(mimfs_fsscandir01_DEPENDENCIES)
	@rm -f mimfs_fsscandir01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
