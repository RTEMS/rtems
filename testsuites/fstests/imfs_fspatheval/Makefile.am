
rtems_tests_PROGRAMS = imfs_fspatheval
imfs_fspatheval_SOURCES  = ../fspatheval/test.c
imfs_fspatheval_SOURCES += ../support/fstest_support.c
imfs_fspatheval_SOURCES += ../support/fstest_support.h
imfs_fspatheval_SOURCES += ../support/fstest.h
imfs_fspatheval_SOURCES += ../../psxtests/include/pmacros.h
imfs_fspatheval_SOURCES += ../imfs_support/fs_support.c
imfs_fspatheval_SOURCES += ../imfs_support/fs_config.h 

#dist_rtems_tests_DATA = imfs_fspatheval.scn
#dist_rtems_tests_DATA += imfs_fspatheval.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/imfs_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(imfs_fspatheval_OBJECTS)
LINK_LIBS = $(imfs_fspatheval_LDLIBS)

imfs_fspatheval$(EXEEXT): $(imfs_fspatheval_OBJECTS) $(imfs_fspatheval_DEPENDENCIES)
	@rm -f imfs_fspatheval$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
