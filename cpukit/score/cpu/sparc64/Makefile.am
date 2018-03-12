include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES =
libscorecpu_a_SOURCES += context.S
libscorecpu_a_SOURCES += cpu.c
libscorecpu_a_SOURCES += interrupt.S
libscorecpu_a_SOURCES += ../no_cpu/cpucounterread.c
libscorecpu_a_SOURCES += sparc64-exception-frame-print.c
libscorecpu_a_SOURCES += sparc64-syscall.S

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
