include $(top_srcdir)/automake/compile.am

CLEANFILES =

noinst_LIBRARIES = libscorecpu.a

libscorecpu_a_SOURCES =
libscorecpu_a_SOURCES += cpu.c
libscorecpu_a_SOURCES += or1k-context-switch.S
libscorecpu_a_SOURCES += or1k-context-initialize.c
libscorecpu_a_SOURCES += or1k-context-validate.S
libscorecpu_a_SOURCES += or1k-context-volatile-clobber.S
libscorecpu_a_SOURCES += or1k-exception-default.c
libscorecpu_a_SOURCES += or1k-exception-frame-print.c
libscorecpu_a_SOURCES += or1k-exception-handler-low.S

libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
