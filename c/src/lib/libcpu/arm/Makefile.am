ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

EXTRA_DIST =

noinst_PROGRAMS =

## shared/include
if shared

## shared/arm920
noinst_PROGRAMS += shared/arm920.rel
shared_arm920_rel_SOURCES = shared/arm920/mmu.c
shared_arm920_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/shared/src
shared_arm920_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(top_srcdir)/../../../automake/local.am
