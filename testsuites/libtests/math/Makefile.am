rtems_tests_PROGRAMS = math
math_SOURCES = init.c domath.c
EXTRA_DIST = $(srcdir)/../math/domath.in

$(srcdir)/domath.c: $(srcdir)/../math/domath.in
	sed -e 's,[@]FTYPE[@],double,' \
	-e 's,[@]FSUFFIX[@], ,g' \
	-e 's,[@]FGUARD[@],NO_DOUBLE,' \
	$(srcdir)/../math/domath.in > $(srcdir)/domath.c

dist_rtems_tests_DATA = math.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

math_LDADD = -lm

LINK_OBJS = $(math_OBJECTS) $(math_LDADD)
LINK_LIBS = $(math_LDLIBS)

math$(EXEEXT): $(math_OBJECTS) $(math_DEPENDENCIES)
	@rm -f math$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
