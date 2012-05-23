rtems_tests_PROGRAMS = mathf
mathf_SOURCES = init.c domathf.c
EXTRA_DIST = $(srcdir)/../math/domath.in

$(srcdir)/domathf.c: $(srcdir)/../math/domath.in
	sed -e 's,[@]FTYPE[@],float,' \
	-e 's,[@]FSUFFIX[@],f,g' \
	-e 's,[@]FGUARD[@],NO_FLOAT,' \
	$(srcdir)/../math/domath.in > $(srcdir)/domathf.c

dist_rtems_tests_DATA = mathf.scn

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

mathf_LDADD = -lm

LINK_OBJS = $(mathf_OBJECTS) $(mathf_LDADD)
LINK_LIBS = $(mathf_LDLIBS)

mathf$(EXEEXT): $(mathf_OBJECTS) $(mathf_DEPENDENCIES)
	@rm -f mathf$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
