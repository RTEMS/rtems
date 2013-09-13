
rtems_tests_PROGRAMS = jffs2_fsrdwr
jffs2_fsrdwr_SOURCES  = ../fsrdwr/init.c
jffs2_fsrdwr_SOURCES += ../support/fstest_support.c
jffs2_fsrdwr_SOURCES += ../support/fstest_support.h
jffs2_fsrdwr_SOURCES += ../support/fstest.h
jffs2_fsrdwr_SOURCES += ../../psxtests/include/pmacros.h
jffs2_fsrdwr_SOURCES += ../jffs2_support/fs_support.c
jffs2_fsrdwr_SOURCES += ../jffs2_support/fs_config.h

#dist_rtems_tests_DATA = jffs2_fsrdwr.scn
#dist_rtems_tests_DATA += jffs2_fsrdwr.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/jffs2_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(jffs2_fsrdwr_OBJECTS)
LINK_LIBS = $(jffs2_fsrdwr_LDLIBS)

jffs2_fsrdwr$(EXEEXT): $(jffs2_fsrdwr_OBJECTS) $(jffs2_fsrdwr_DEPENDENCIES)
	@rm -f jffs2_fsrdwr$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
