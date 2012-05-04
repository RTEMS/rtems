if HAS_COMPLEX
rtems_tests_PROGRAMS = complex
complex_SOURCES = init.c docomplex.c docomplexf.c docomplexl.c
endif
EXTRA_DIST = docomplex.in

# FIXME: Skip long double, not yet supported in newlib
complex_CPPFLAGS = $(AM_CPPFLAGS) -DNO_LONG_DOUBLE

docomplex.c: $(srcdir)/docomplex.in
	sed -e 's,[@]FTYPE[@],double,' \
	-e 's,[@]FSUFFIX[@], ,g' \
	-e 's,[@]FGUARD[@],NO_DOUBLE,' \
	$(srcdir)/docomplex.in > $(srcdir)/docomplex.c

docomplexf.c: $(srcdir)/docomplex.in
	sed -e 's,[@]FTYPE[@],float,' \
	-e 's,[@]FSUFFIX[@],f,g' \
	-e 's,[@]FGUARD[@],NO_FLOAT,' \
	$(srcdir)/docomplex.in > $(srcdir)/docomplexf.c

docomplexl.c: $(srcdir)/docomplex.in
	sed -e 's,[@]FTYPE[@],long double,' \
	-e 's,[@]FSUFFIX[@],l,g' \
	-e 's,%f,%Lf,g' \
	-e 's,[@]FGUARD[@],NO_LONG_DOUBLE,' \
	$(srcdir)/docomplex.in > $(srcdir)/docomplexl.c

dist_rtems_tests_DATA = complex.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

if HAS_COMPLEX
complex_LDADD = -lm

LINK_OBJS = $(complex_OBJECTS) $(complex_LDADD)
LINK_LIBS = $(complex_LDLIBS)

complex$(EXEEXT): $(complex_OBJECTS) $(complex_DEPENDENCIES)
	@rm -f complex$(EXEEXT)
	$(make-exe)
endif

include $(top_srcdir)/../automake/local.am
