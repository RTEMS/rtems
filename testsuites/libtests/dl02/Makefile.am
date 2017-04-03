rtems_tests_PROGRAMS = dl02
dl02_SOURCES = init.c dl-load.c dl-tar.c dl-tar.h

BUILT_SOURCES = dl-tar.c dl-tar.h

dist_rtems_tests_DATA = dl02.scn dl02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(dl02_OBJECTS)
LINK_LIBS = $(dl02_LDLIBS)

dl-o1.o: dl-o1.c

dl-o2.o: dl-o2.c

dl.tar: dl-o1.o dl-o2.o
	@rm -f $@
	$(PAX) -w -f $@ $^
CLEANFILES += dl.tar

dl-tar.c: dl.tar
	$(BIN2C) -C $< $@
CLEANFILES += dl-tar.c

dl-tar.h: dl.tar
	$(BIN2C) -H $< $@
CLEANFILES += dl-tar.h

dl02.pre: $(dl02_OBJECTS) $(dl02_DEPENDENCIES)
	@rm -f dl02.pre
	$(LINK_APP)

dl-sym.o: dl02.pre
	rtems-syms -e -c "$(CFLAGS)" -o $@ $<

dl02$(EXEEXT):  $(dl02_OBJECTS) $(dl02_DEPENDENCIES) dl-sym.o
	@rm -f dl02$(EXEEXT)
	$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) \
		    -o $(basename $@)$(EXEEXT) $(LINK_OBJS) dl-sym.o $(LINK_LIBS)

include $(top_srcdir)/../automake/local.am
