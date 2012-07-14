rtems_tests_PROGRAMS = mghttpd01

mghttpd01_SOURCES = init.c init_fs.c init_fs.h \
	test-http-client.c test-http-client.h
mghttpd01_LDADD = -lmghttpd
BUILT_SOURCES = init_fs.c init_fs.h

dist_rtems_tests_DATA = mghttpd01.scn mghttpd01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(mghttpd01_OBJECTS) $(mghttpd01_LDADD)
LINK_LIBS = $(mghttpd01_LDLIBS)

mghttpd01$(EXEEXT): $(mghttpd01_OBJECTS) $(mghttpd01_DEPENDENCIES)
	@rm -f mghttpd01$(EXEEXT)
	$(make-exe)

init_fs.c: 
	$(BIN2C) -C $(srcdir)/init_fs.tar init_fs
CLEANFILES += init_fs.c
	
init_fs.h:
	$(BIN2C) -H $(srcdir)/init_fs.tar init_fs
CLEANFILES += init_fs.h

include $(top_srcdir)/../automake/local.am
