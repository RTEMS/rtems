rtems_tests_PROGRAMS = mathl
mathl_SOURCES = init.c domathl.c
EXTRA_DIST = $(srcdir)/../math/domath.in

# FIXME: Skip long double, not yet supported in newlib
# => This test currently is a nop
mathl_CPPFLAGS = $(AM_CPPFLAGS) -DNO_LONG_DOUBLE

$(srcdir)/domathl.c: $(srcdir)/../math/domath.in
	sed -e 's,[@]FTYPE[@],long double,' \
	-e 's,[@]FSUFFIX[@],l,g' \
	-e 's,%f,%Lf,g' \
	-e 's,[@]FGUARD[@],NO_LONG_DOUBLE,' \
	$(srcdir)/../math/domath.in > $(srcdir)/domathl.c

dist_rtems_tests_DATA = mathl.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

mathl_LDADD = -lm

LINK_OBJS = $(mathl_OBJECTS) $(mathl_LDADD)
LINK_LIBS = $(mathl_LDLIBS)

mathl$(EXEEXT): $(mathl_OBJECTS) $(mathl_DEPENDENCIES)
	@rm -f mathl$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
