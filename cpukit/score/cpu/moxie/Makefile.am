##
## $Id: Makefile.am,v 1.35 2006/01/12 09:57:43 ralf Exp $
##

include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c moxie-exception-frame-print.c cpu_asm.S
libscorecpu_a_SOURCES += ../no_cpu/cpucounterread.c
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
