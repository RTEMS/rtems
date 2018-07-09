include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES  = cpu.c
libscorecpu_a_SOURCES += ../no_cpu/cpucounterfrequency.c
libscorecpu_a_SOURCES += ../no_cpu/cpucounterread.c
libscorecpu_a_SOURCES += x86_64-context-initialize.c
libscorecpu_a_SOURCES += x86_64-context-switch.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
