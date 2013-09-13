
rtems_tests_PROGRAMS = jffs2_fssymlink
jffs2_fssymlink_SOURCES  = ../fssymlink/test.c
jffs2_fssymlink_SOURCES += ../support/fstest_support.c
jffs2_fssymlink_SOURCES += ../support/fstest_support.h
jffs2_fssymlink_SOURCES += ../support/fstest.h
jffs2_fssymlink_SOURCES += ../../psxtests/include/pmacros.h
jffs2_fssymlink_SOURCES += ../jffs2_support/fs_support.c
jffs2_fssymlink_SOURCES += ../jffs2_support/fs_config.h

#dist_rtems_tests_DATA = jffs2_fssymlink.scn
#dist_rtems_tests_DATA += jffs2_fssymlink.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/jffs2_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(jffs2_fssymlink_OBJECTS)
LINK_LIBS = $(jffs2_fssymlink_LDLIBS)

jffs2_fssymlink$(EXEEXT): $(jffs2_fssymlink_OBJECTS) $(jffs2_fssymlink_DEPENDENCIES)
	@rm -f jffs2_fssymlink$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
