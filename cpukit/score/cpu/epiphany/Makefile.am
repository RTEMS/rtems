include $(top_srcdir)/automake/compile.am

CLEANFILES =

noinst_LIBRARIES = libscorecpu.a

libscorecpu_a_SOURCES  = cpu.c
libscorecpu_a_SOURCES += epiphany-exception-handler.S
libscorecpu_a_SOURCES += epiphany-context-switch.S
libscorecpu_a_SOURCES += epiphany-context-initialize.c
libscorecpu_a_SOURCES += setjmp.S

libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
