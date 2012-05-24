if HAS_COMPLEX
rtems_tests_PROGRAMS = complex
complex_SOURCES = init.c docomplex.c docomplexf.c docomplexl.c
endif

# FIXME: Skip long double, not yet supported in newlib
complex_CPPFLAGS = $(AM_CPPFLAGS) -DNO_LONG_DOUBLE

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
