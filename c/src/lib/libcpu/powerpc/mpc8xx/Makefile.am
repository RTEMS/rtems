##
## $Id$
##

CLEANFILES =
EXTRA_DIST =
EXTRA_PROGRAMS =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am

if mpc8xx
include_mpc8xxdir = $(includedir)/mpc8xx
include_libcpudir = $(includedir)/libcpu

include_HEADERS = include/mpc8xx.h

## clock
EXTRA_PROGRAMS += clock.rel
CLEANFILES += clock.rel
clock_rel_SOURCES = clock/clock.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += clock_g.rel
CLEANFILES += clock_g.rel
clock_g_rel_SOURCES = $(clock_rel_SOURCES)
clock_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
clock_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += clock$(LIB_VARIANT).rel

## console-generic
include_mpc8xx_HEADERS = include/console.h

EXTRA_PROGRAMS += console-generic.rel
CLEANFILES += console-generic.rel
console_generic_rel_SOURCES = console-generic/console-generic.c
console_generic_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
console_generic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += console-generic_g.rel
CLEANFILES += console-generic_g.rel
console_generic_g_rel_SOURCES = $(console_generic_rel_SOURCES)
console_generic_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
console_generic_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += console-generic$(LIB_VARIANT).rel

## cpm
include_mpc8xx_HEADERS += include/cpm.h

EXTRA_PROGRAMS += cpm.rel
CLEANFILES += cpm.rel
cpm_rel_SOURCES = cpm/cp.c cpm/dpram.c
cpm_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
cpm_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += cpm_g.rel
CLEANFILES += cpm_g.rel
cpm_g_rel_SOURCES = $(cpm_rel_SOURCES)
cpm_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
cpm_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += cpm$(LIB_VARIANT).rel

## exceptions
include_libcpu_HEADERS = exceptions/raw_exception.h

EXTRA_PROGRAMS += exceptions.rel
CLEANFILES += exceptions.rel
exceptions_rel_SOURCES = exceptions/raw_exception.c exceptions/asm_utils.S
exceptions_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
exceptions_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += exceptions_g.rel
CLEANFILES += exceptions_g.rel
exceptions_g_rel_SOURCES = $(exceptions_rel_SOURCES)
exceptions_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
exceptions_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += exceptions$(LIB_VARIANT).rel

## mmu
include_mpc8xx_HEADERS += include/mmu.h

EXTRA_PROGRAMS += mmu.rel
CLEANFILES += mmu.rel
mmu_rel_SOURCES = mmu/mmu.c
mmu_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
mmu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += mmu_g.rel
CLEANFILES += mmu_g.rel
mmu_g_rel_SOURCES = $(mmu_rel_SOURCES)
mmu_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
mmu_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += mmu$(LIB_VARIANT).rel

## timer
EXTRA_PROGRAMS += timer.rel
CLEANFILES += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += timer_g.rel
CLEANFILES += timer_g.rel
timer_g_rel_SOURCES = $(timer_rel_SOURCES)
timer_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
timer_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += timer$(LIB_VARIANT).rel
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
