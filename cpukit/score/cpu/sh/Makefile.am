include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c context.c
libscorecpu_a_SOURCES += ../no_cpu/cpucounterread.c
libscorecpu_a_SOURCES += sh-exception-frame-print.c
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
