rtems_tests_PROGRAMS = dl01
dl01_SOURCES = init.c dl-load.c dl-tar.c dl-tar.h

BUILT_SOURCES = dl-tar.c dl-tar.h

dist_rtems_tests_DATA = dl01.scn dl01.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(dl01_OBJECTS)
LINK_LIBS = $(dl01_LDLIBS)

dl-o1.o: dl-o1.c

dl.tar: dl-o1.o
	@rm -f $@
	$(PAX) -w -f $@ $<
CLEANFILES += dl.tar

dl-tar.c: dl.tar
	$(BIN2C) -C $< $@
CLEANFILES += dl-tar.c

dl-tar.h: dl.tar
	$(BIN2C) -H $< $@
CLEANFILES += dl-tar.h

dl01.pre: $(dl01_OBJECTS) $(dl01_DEPENDENCIES)
	@rm -f dl01.pre
	$(LINK_APP)

dl-sym.o: dl01.pre
	rtems-syms -e -c "$(CFLAGS)" -o $@ $<

dl01$(EXEEXT):  $(dl01_OBJECTS) $(dl01_DEPENDENCIES) dl-sym.o
	@rm -f dl01$(EXEEXT)
	$(LINK.c) $(CPU_CFLAGS) $(AM_CFLAGS) $(AM_LDFLAGS) \
		    -o $(basename $@)$(EXEEXT) $(LINK_OBJS) dl-sym.o $(LINK_LIBS)

include $(top_srcdir)/../automake/local.am
