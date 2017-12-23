include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c
libscorecpu_a_SOURCES += ppc-context-volatile-clobber.S
libscorecpu_a_SOURCES += ppc-context-validate.S
libscorecpu_a_SOURCES += ppc-isr-disable-mask.S
libscorecpu_a_SOURCES += ppc-isr-vector-install.c
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
