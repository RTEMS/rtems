##
## Makefile.am,v 1.4 2002/03/28 00:48:14 joel Exp
##

CLEANFILES =
EXTRA_DIST =
noinst_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am

if mpc5xx
include_mpc5xxdir = $(includedir)/mpc5xx
include_libcpudir = $(includedir)/libcpu

include_HEADERS = include/mpc5xx.h

## clock
noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/clock.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## console-generic
include_mpc5xx_HEADERS = include/console.h

noinst_PROGRAMS += console-generic.rel
console_generic_rel_SOURCES = console-generic/console-generic.c
console_generic_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_generic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## exceptions
include_libcpu_HEADERS = exceptions/raw_exception.h

noinst_PROGRAMS += exceptions.rel
exceptions_rel_SOURCES = exceptions/raw_exception.c
exceptions_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
exceptions_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## irq
include_libcpu_HEADERS += irq/irq.h

noinst_PROGRAMS += irq.rel
irq_rel_SOURCES = irq/irq.c irq/irq_init.c irq/irq_asm.S
irq_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## timer
noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## vectors
include_libcpu_HEADERS += vectors/vectors.h

noinst_PROGRAMS += vectors.rel
vectors_rel_SOURCES = vectors/vectors_init.c vectors/vectors.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

## --
all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

if mpc5xx
$(PROJECT_INCLUDE)/mpc5xx/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/mpc5xx
	@: > $(PROJECT_INCLUDE)/mpc5xx/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/mpc5xx/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/mpc5xx.h: include/mpc5xx.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc5xx.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc5xx.h

$(PROJECT_INCLUDE)/mpc5xx/console.h: include/console.h $(PROJECT_INCLUDE)/mpc5xx/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc5xx/console.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc5xx/console.h

$(PROJECT_INCLUDE)/libcpu/raw_exception.h: exceptions/raw_exception.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/raw_exception.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/raw_exception.h

$(PROJECT_INCLUDE)/libcpu/irq.h: irq/irq.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/irq.h

$(PROJECT_INCLUDE)/libcpu/vectors.h: vectors/vectors.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/vectors.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/vectors.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
