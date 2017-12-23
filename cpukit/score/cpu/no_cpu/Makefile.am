include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c cpu_asm.c
libscorecpu_a_SOURCES += cpucounterread.c
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
