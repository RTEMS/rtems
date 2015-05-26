
rtems_tests_PROGRAMS = jffs2_fsscandir01
jffs2_fsscandir01_SOURCES  = ../fsscandir01/init.c
jffs2_fsscandir01_SOURCES += ../support/fstest_support.c
jffs2_fsscandir01_SOURCES += ../support/fstest_support.h
jffs2_fsscandir01_SOURCES += ../support/fstest.h
jffs2_fsscandir01_SOURCES += ../../psxtests/include/pmacros.h
jffs2_fsscandir01_SOURCES += ../jffs2_support/fs_support.c
jffs2_fsscandir01_SOURCES += ../jffs2_support/fs_config.h
jffs2_fsscandir01_LDADD = -ljffs2

dist_rtems_tests_DATA = jffs2_fsscandir01.scn
#dist_rtems_tests_DATA += jffs2_fsscandir01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


AM_CPPFLAGS += -I$(top_srcdir)/support
AM_CPPFLAGS += -I$(top_srcdir)/jffs2_support
AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/../psxtests/include

LINK_OBJS = $(jffs2_fsscandir01_OBJECTS) $(jffs2_fsscandir01_LDADD)
LINK_LIBS = $(jffs2_fsscandir01_LDLIBS)

jffs2_fsscandir01$(EXEEXT): $(jffs2_fsscandir01_OBJECTS) $(jffs2_fsscandir01_DEPENDENCIES)
	@rm -f jffs2_fsscandir01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
