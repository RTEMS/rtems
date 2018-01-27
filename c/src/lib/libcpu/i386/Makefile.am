ACLOCAL_AMFLAGS = -I ../../../aclocal

noinst_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am

noinst_PROGRAMS += score.rel
score_rel_SOURCES = displayCpu.c cpuModel.S cpuModel.h idtr.S cpu.h
score_rel_CPPFLAGS = $(AM_CPPFLAGS)
score_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += page.rel
page_rel_SOURCES = page.c page.h
page_rel_CPPFLAGS = $(AM_CPPFLAGS)
page_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include $(top_srcdir)/../../../automake/local.am
