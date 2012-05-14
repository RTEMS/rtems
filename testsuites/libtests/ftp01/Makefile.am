
rtems_tests_PROGRAMS = ftp01
ftp01_SOURCES = init.c
ftp01_LDADD = -lftpd

dist_rtems_tests_DATA = ftp01.scn
dist_rtems_tests_DATA += ftp01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include
AM_CPPFLAGS += -I$(top_srcdir)/termios04

LINK_OBJS = $(ftp01_OBJECTS) $(ftp01_LDADD)
LINK_LIBS = $(ftp01_LDLIBS)

ftp01$(EXEEXT): $(ftp01_OBJECTS) $(ftp01_DEPENDENCIES)
	@rm -f ftp01$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
