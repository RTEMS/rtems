##
## $Id$
##

EXTRA_DIST =
noinst_PROGRAMS =
CLEANFILES =

include $(top_srcdir)/../../../automake/compile.am

if mpc8260
include_mpc8260dir = $(includedir)/mpc8260

include_HEADERS = include/mpc8260.h

include_libcpudir = $(includedir)/libcpu

## console-generic
include_mpc8260_HEADERS = include/console.h

noinst_PROGRAMS += console-generic.rel
console_generic_rel_SOURCES = console-generic/console-generic.c \
    include/console.h
console_generic_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_generic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## clock
noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/clock.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## cpm
include_mpc8260_HEADERS += include/cpm.h

noinst_PROGRAMS += cpm.rel
cpm_rel_SOURCES = cpm/cp.c cpm/dpram.c cpm/brg.c include/cpm.h
cpm_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
cpm_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## exceptions
include_libcpu_HEADERS = exceptions/raw_exception.h

noinst_PROGRAMS += exceptions.rel
exceptions_rel_SOURCES = exceptions/raw_exception.c exceptions/asm_utils.S \
    exceptions/raw_exception.h
exceptions_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
exceptions_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## mmu
include_mpc8260_HEADERS += include/mmu.h

noinst_PROGRAMS += mmu.rel
mmu_rel_SOURCES = mmu/mmu.c include/mmu.h
mmu_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
mmu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## timer
noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

all-local: $(PREINSTALL_FILES)

## --

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

if mpc8260
$(PROJECT_INCLUDE)/mpc8260/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/mpc8260
	@: > $(PROJECT_INCLUDE)/mpc8260/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/mpc8260/$(dirstamp)

$(PROJECT_INCLUDE)/mpc8260.h: include/mpc8260.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8260.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8260.h

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/mpc8260/console.h: include/console.h $(PROJECT_INCLUDE)/mpc8260/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8260/console.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8260/console.h

$(PROJECT_INCLUDE)/mpc8260/cpm.h: include/cpm.h $(PROJECT_INCLUDE)/mpc8260/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8260/cpm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8260/cpm.h

$(PROJECT_INCLUDE)/libcpu/raw_exception.h: exceptions/raw_exception.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/raw_exception.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/raw_exception.h

$(PROJECT_INCLUDE)/mpc8260/mmu.h: include/mmu.h $(PROJECT_INCLUDE)/mpc8260/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8260/mmu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8260/mmu.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
