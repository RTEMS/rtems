include $(top_srcdir)/automake/compile.am
noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)
libscorecpu_a_SOURCES  = cpu.c
libscorecpu_a_SOURCES += riscv-exception-handler.S
libscorecpu_a_SOURCES += riscv-exception-default.c
libscorecpu_a_SOURCES += riscv-exception-frame-print.c
libscorecpu_a_SOURCES += riscv-context-switch.S
libscorecpu_a_SOURCES += riscv-context-initialize.c
libscorecpu_a_SOURCES += riscv-context-validate.S
libscorecpu_a_SOURCES += riscv-context-volatile-clobber.S

include $(top_srcdir)/automake/local.am
include $(srcdir)/headers.am
