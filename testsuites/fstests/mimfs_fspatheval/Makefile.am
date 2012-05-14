
rtems_tests_PROGRAMS = mimfs_fspatheval
mimfs_fspatheval_SOURCES  = ../fspatheval/test.c
mimfs_fspatheval_SOURCES += ../support/fstest_support.c
mimfs_fspatheval_SOURCES += ../support/fstest_support.h
mimfs_fspatheval_SOURCES += ../support/fstest.h
mimfs_fspatheval_SOURCES += ../../psxtests/include/pmacros.h
mimfs_fspatheval_SOURCES += ../mimfs_support/fs_support.c
mimfs_fspatheval_SOURCES += ../mimfs_support/fs_config.h

#dist_rtems_tests_DATA = mimfs_fspatheval.scn
#dist_rtems_tests_DATA += mimfs_fspatheval.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/mimfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(mimfs_fspatheval_OBJECTS)
LINK_LIBS = $(mimfs_fspatheval_LDLIBS)

mimfs_fspatheval$(EXEEXT): $(mimfs_fspatheval_OBJECTS) $(mimfs_fspatheval_DEPENDENCIES)
	@rm -f mimfs_fspatheval$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
