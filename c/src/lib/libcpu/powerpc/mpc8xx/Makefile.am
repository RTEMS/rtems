##
## $Id$
##

CLEANFILES =
EXTRA_DIST =
noinst_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am

if mpc8xx
include_mpc8xxdir = $(includedir)/mpc8xx
include_libcpudir = $(includedir)/libcpu

include_HEADERS = include/mpc8xx.h

## clock
noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/clock.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## console-generic
include_mpc8xx_HEADERS = include/console.h

noinst_PROGRAMS += console-generic.rel
console_generic_rel_SOURCES = console-generic/console-generic.c
console_generic_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_generic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## cpm
include_mpc8xx_HEADERS += include/cpm.h

noinst_PROGRAMS += cpm.rel
cpm_rel_SOURCES = cpm/cp.c cpm/dpram.c
cpm_rel_CPPFLAGS = $(AM_CPPFLAGS)
cpm_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## exceptions
include_libcpu_HEADERS = exceptions/raw_exception.h

noinst_PROGRAMS += exceptions.rel
exceptions_rel_SOURCES = exceptions/raw_exception.c exceptions/asm_utils.S
exceptions_rel_CPPFLAGS = $(AM_CPPFLAGS)
exceptions_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## mmu
include_mpc8xx_HEADERS += include/mmu.h

noinst_PROGRAMS += mmu.rel
mmu_rel_SOURCES = mmu/mmu.c
mmu_rel_CPPFLAGS = $(AM_CPPFLAGS)
mmu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## timer
noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

## --
all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

if mpc8xx
$(PROJECT_INCLUDE)/mpc8xx/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/mpc8xx
	@: > $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/mpc8xx.h: include/mpc8xx.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx.h

$(PROJECT_INCLUDE)/mpc8xx/console.h: include/console.h $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx/console.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx/console.h

$(PROJECT_INCLUDE)/mpc8xx/cpm.h: include/cpm.h $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx/cpm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx/cpm.h

$(PROJECT_INCLUDE)/libcpu/raw_exception.h: exceptions/raw_exception.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/raw_exception.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/raw_exception.h

$(PROJECT_INCLUDE)/mpc8xx/mmu.h: include/mmu.h $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx/mmu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx/mmu.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
