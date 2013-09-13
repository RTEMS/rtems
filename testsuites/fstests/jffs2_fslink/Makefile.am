
rtems_tests_PROGRAMS = jffs2_fslink
jffs2_fslink_SOURCES  = ../fslink/test.c
jffs2_fslink_SOURCES += ../support/fstest_support.c
jffs2_fslink_SOURCES += ../support/fstest_support.h
jffs2_fslink_SOURCES += ../support/fstest.h
jffs2_fslink_SOURCES += ../../psxtests/include/pmacros.h
jffs2_fslink_SOURCES += ../jffs2_support/fs_support.c
jffs2_fslink_SOURCES += ../jffs2_support/fs_config.h

#dist_rtems_tests_DATA = jffs2_fslink.scn
#dist_rtems_tests_DATA += jffs2_fslink.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/jffs2_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(jffs2_fslink_OBJECTS)
LINK_LIBS = $(jffs2_fslink_LDLIBS)

jffs2_fslink$(EXEEXT): $(jffs2_fslink_OBJECTS) $(jffs2_fslink_DEPENDENCIES)
	@rm -f jffs2_fslink$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
