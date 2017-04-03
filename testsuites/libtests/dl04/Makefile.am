rtems_tests_PROGRAMS = dl04
# include a C++ file in the source to trick automake into adding C++ rules <sigh>.
dl04_SOURCES = init.c dl-load.c dl-cpp.cpp dl-tar.c dl-tar.h

BUILT_SOURCES = dl-tar.c dl-tar.h

dist_rtems_tests_DATA = dl04.scn dl04.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(dl04_OBJECTS)
LINK_LIBS = $(dl04_LDLIBS)

dl-o4.o: dl-o4.cpp

dl.tar: dl-o4.o
	@rm -f $@
	$(PAX) -w -f $@ $<
CLEANFILES += dl.tar

dl-tar.c: dl.tar
	$(BIN2C) -C $< $@
CLEANFILES += dl-tar.c

dl-tar.h: dl.tar
	$(BIN2C) -H $< $@
CLEANFILES += dl-tar.h

dl04.pre: $(dl04_OBJECTS) $(dl04_DEPENDENCIES)
	@rm -f dl04.pre
	$(LINK_APP)

dl-sym.o: dl04.pre
	rtems-syms -e -c "$(CFLAGS)" -o $@ $<

dl04$(EXEEXT):  $(dl04_OBJECTS) $(dl04_DEPENDENCIES) dl-sym.o
	@rm -f dl04$(EXEEXT)
	$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) \
		    -o $(basename $@)$(EXEEXT) $(LINK_OBJS) dl-sym.o $(LINK_LIBS)

include $(top_srcdir)/../automake/local.am
