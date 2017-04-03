rtems_tests_PROGRAMS = dl05
dl05_SOURCES = init.c dl-load.c dl-cpp.cpp dl-tar.c dl-tar.h

BUILT_SOURCES = dl-tar.c dl-tar.h

dist_rtems_tests_DATA = dl05.scn dl05.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(dl05_OBJECTS)
LINK_LIBS = $(dl05_LDLIBS)

dl-o5.o: dl-o5.cpp

dl.tar: dl-o5.o
	@rm -f $@
	$(PAX) -w -f $@ $<
CLEANFILES += dl.tar

dl-tar.c: dl.tar
	$(BIN2C) -C $< $@
CLEANFILES += dl-tar.c

dl-tar.h: dl.tar
	$(BIN2C) -H $< $@
CLEANFILES += dl-tar.h

dl05.pre: $(dl05_OBJECTS) $(dl05_DEPENDENCIES)
	@rm -f dl05.pre
	$(LINK.cc) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) \
		    -o $@ $(LINK_OBJS) $(LINK_LIBS)

dl-sym.o: dl05.pre
	rtems-syms -e -c "$(CFLAGS)" -o $@ $<

dl05$(EXEEXT):  $(dl05_OBJECTS) $(dl05_DEPENDENCIES) dl-sym.o
	@rm -f dl05$(EXEEXT)
	$(LINK.cc) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) \
		    -o $(basename $@)$(EXEEXT) $(LINK_OBJS) dl-sym.o $(LINK_LIBS)

include $(top_srcdir)/../automake/local.am
