##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

if i960ka
EXTRA_PROGRAMS += score.rel
CLEANFILES += score.rel
score_rel_SOURCES = cpu_install_intr_stack.c cpu_install_raw_isr.c
score_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
score_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += score_g.rel
CLEANFILES += score_g.rel
score_g_rel_SOURCES = $(score_rel_SOURCES)
score_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
score_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += score$(LIB_VARIANT).rel
endif

include $(top_srcdir)/../../../automake/local.am
