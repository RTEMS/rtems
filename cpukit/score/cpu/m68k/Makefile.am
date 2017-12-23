include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

libscorecpu_a_SOURCES = cpu.c cpu_asm.S
libscorecpu_a_SOURCES += ../no_cpu/cpucounterread.c
libscorecpu_a_SOURCES += m68k-exception-frame-print.c
libscorecpu_a_SOURCES += __m68k_read_tp.c

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
