include $(top_srcdir)/automake/compile.am

#include_rtems_sparc64dir = $(includedir)/rtems/sparc64
#include_rtems_sparc64_HEADERS = 

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = context.S cpu.c
libscorecpu_a_SOURCES += ../no_cpu/cpucounterread.c
libscorecpu_a_SOURCES += sparc64-exception-frame-print.c
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
