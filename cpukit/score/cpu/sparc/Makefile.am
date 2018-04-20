include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES =
libscorecpu_a_SOURCES += access_le.c
libscorecpu_a_SOURCES += access.S
libscorecpu_a_SOURCES += cpu.c
libscorecpu_a_SOURCES += cpu_asm.S
libscorecpu_a_SOURCES += sparc-context-validate.S
libscorecpu_a_SOURCES += sparc-context-volatile-clobber.S
libscorecpu_a_SOURCES += sparc-counter-asm.S
libscorecpu_a_SOURCES += sparc-counter.c
libscorecpu_a_SOURCES += syscall.S
libscorecpu_a_SOURCES += window.S

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
