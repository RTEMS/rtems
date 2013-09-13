
rtems_tests_PROGRAMS = jffs2_fstime
jffs2_fstime_SOURCES  = ../fstime/test.c
jffs2_fstime_SOURCES += ../support/fstest_support.c
jffs2_fstime_SOURCES += ../support/fstest_support.h
jffs2_fstime_SOURCES += ../support/fstest.h
jffs2_fstime_SOURCES += ../../psxtests/include/pmacros.h
jffs2_fstime_SOURCES += ../jffs2_support/fs_support.c
jffs2_fstime_SOURCES += ../jffs2_support/fs_config.h

#dist_rtems_tests_DATA = jffs2_fstime.scn
#dist_rtems_tests_DATA += jffs2_fstime.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/jffs2_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(jffs2_fstime_OBJECTS)
LINK_LIBS = $(jffs2_fstime_LDLIBS)

jffs2_fstime$(EXEEXT): $(jffs2_fstime_OBJECTS) $(jffs2_fstime_DEPENDENCIES)
	@rm -f jffs2_fstime$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
