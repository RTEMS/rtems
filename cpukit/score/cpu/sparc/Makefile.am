include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c
libscorecpu_a_SOURCES += sparc-context-volatile-clobber.S
libscorecpu_a_SOURCES += sparc-context-validate.S
libscorecpu_a_SOURCES += sparc-counter.c
libscorecpu_a_SOURCES += sparc-counter-asm.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
