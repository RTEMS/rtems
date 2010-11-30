##
##  $Id$
##

rtems_tests_PROGRAMS = math
math_SOURCES = init.c domath.c domathf.c domathl.c
EXTRA_DIST = domath.in

# FIXME: Skip long double, not yet supported in newlib
math_CPPFLAGS = $(AM_CPPFLAGS) -DNO_LONG_DOUBLE

domath.c: $(srcdir)/domath.in
	sed -e 's,[@]FTYPE[@],double,' \
	-e 's,[@]FSUFFIX[@], ,g' \
	-e 's,[@]FGUARD[@],NO_DOUBLE,' \
	$(srcdir)/domath.in > $(srcdir)/domath.c

domathf.c: $(srcdir)/domath.in
	sed -e 's,[@]FTYPE[@],float,' \
	-e 's,[@]FSUFFIX[@],f,g' \
	-e 's,[@]FGUARD[@],NO_FLOAT,' \
	$(srcdir)/domath.in > $(srcdir)/domathf.c

domathl.c: $(srcdir)/domath.in
	sed -e 's,[@]FTYPE[@],long double,' \
	-e 's,[@]FSUFFIX[@],l,g' \
	-e 's,%f,%Lf,g' \
	-e 's,[@]FGUARD[@],NO_LONG_DOUBLE,' \
	$(srcdir)/domath.in > $(srcdir)/domathl.c

dist_rtems_tests_DATA = math.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

math_LDADD = $(MANAGERS_NOT_WANTED:%=$(PROJECT_LIB)/no-%.rel) -lm

LINK_OBJS = $(math_OBJECTS) $(math_LDADD)
LINK_LIBS = $(math_LDLIBS)

math$(EXEEXT): $(math_OBJECTS) $(math_DEPENDENCIES)
	@rm -f math$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
