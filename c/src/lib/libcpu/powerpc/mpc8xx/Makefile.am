##
## $Id$
##

CLEANDIRS =
EXTRA_DIST =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include_mpc8xxdir = $(includedir)/mpc8xx

include_HEADERS = include/mpc8xx.h
include_mpc8xx_HEADERS = include/console.h include/cpm.h include/mmu.h

include_libcpudir = $(includedir)/libcpu
include_libcpu_HEADERS = exceptions/raw_exception.h

## clock
clock_C_FILES = clock/clock.c
EXTRA_DIST += $(clock_C_FILES)

clock_rel_OBJECTS = $(clock_C_FILES:clock/%.c=clock/${ARCH}/%.$(OBJEXT))

clock/$(ARCH)/$(dirstamp):
	@$(mkdir_p) clock/$(ARCH)
	@: > clock/$(ARCH)/$(dirstamp)
clock/$(ARCH)/%.$(OBJEXT): clock/%.c clock/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
clock/$(ARCH)/clock.rel: $(clock_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += clock/o-optimize clock/o-debug
noinst_DATA += clock/$(ARCH)/clock.rel

## console-generic
console_C_FILES = console-generic/console-generic.c
EXTRA_DIST += $(console_C_FILES)

console_rel_OBJECTS = \
    $(console_C_FILES:console-generic/%.c=console-generic/${ARCH}/%.$(OBJEXT))

console-generic/$(ARCH)/$(dirstamp):
	@$(mkdir_p) console-generic/$(ARCH)
	@: > console-generic/$(ARCH)/$(dirstamp)
console-generic/$(ARCH)/%.$(OBJEXT): console-generic/%.c console-generic/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
console-generic/$(ARCH)/console-generic.rel: $(console_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += console-generic/o-optimize console-generic/o-debug
noinst_DATA += console-generic/$(ARCH)/console-generic.rel

## cpm
cpm_C_FILES = cpm/cp.c cpm/dpram.c
EXTRA_DIST += $(cpm_C_FILES)

cpm_rel_OBJECTS = $(cpm_C_FILES:cpm/%.c=cpm/${ARCH}/%.$(OBJEXT))

cpm/$(ARCH)/$(dirstamp):
	@$(mkdir_p) cpm/$(ARCH)
	@: > cpm/$(ARCH)/$(dirstamp)
cpm/$(ARCH)/%.$(OBJEXT): cpm/%.c cpm/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
cpm/$(ARCH)/cpm.rel: $(cpm_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += cpm/o-optimize cpm/o-debug
noinst_DATA += cpm/$(ARCH)/cpm.rel

## exceptions
exceptions_C_FILES = exceptions/raw_exception.c
exceptions_S_FILES = exceptions/asm_utils.S
EXTRA_DIST += $(exceptions_C_FILES) $(exceptions_S_FILES)

exceptions_rel_OBJECTS = \
    $(exceptions_C_FILES:exceptions/%.c=exceptions/$(ARCH)/%.$(OBJEXT)) \
    $(exceptions_S_FILES:exceptions/%.S=exceptions/$(ARCH)/%.$(OBJEXT))

exceptions/$(ARCH)/$(dirstamp):
	@$(mkdir_p) exceptions/$(ARCH)
	@: > exceptions/$(ARCH)/$(dirstamp)
exceptions/$(ARCH)/%.$(OBJEXT): exceptions/%.c exceptions/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
exceptions/$(ARCH)/%.$(OBJEXT): exceptions/%.S exceptions/$(ARCH)/$(dirstamp)
	${CCASCOMPILE} -o $@ -c $<
exceptions/$(ARCH)/exceptions.rel: $(exceptions_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += exceptions/o-optimize exceptions/o-debug
noinst_DATA += exceptions/$(ARCH)/exceptions.rel

## mmu
mmu_C_FILES = mmu/mmu.c
EXTRA_DIST += $(mmu_C_FILES)

mmu_rel_OBJECTS = $(mmu_C_FILES:mmu/%.c=mmu/${ARCH}/%.$(OBJEXT))

mmu/$(ARCH)/$(dirstamp):
	@$(mkdir_p) mmu/$(ARCH)
	@: > mmu/$(ARCH)/$(dirstamp)
mmu/$(ARCH)/%.$(OBJEXT): mmu/%.c mmu/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
mmu/$(ARCH)/mmu.rel: $(mmu_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += mmu/o-optimize mmu/o-debug
noinst_DATA += mmu/$(ARCH)/mmu.rel

## timer
timer_C_FILES = timer/timer.c
EXTRA_DIST += $(timer_C_FILES)

timer_rel_OBJECTS = $(timer_C_FILES:timer/%.c=timer/${ARCH}/%.$(OBJEXT))

timer/$(ARCH)/$(dirstamp):
	@$(mkdir_p) timer/$(ARCH)
	@: > timer/$(ARCH)/$(dirstamp)
timer/$(ARCH)/%.$(OBJEXT): timer/%.c timer/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
timer/$(ARCH)/timer.rel: $(timer_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += timer/o-optimize timer/o-debug
noinst_DATA += timer/$(ARCH)/timer.rel

## --
all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/mpc8xx/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/mpc8xx
	@: > $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)

$(PROJECT_INCLUDE)/mpc8xx.h: include/mpc8xx.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx.h

$(PROJECT_INCLUDE)/mpc8xx/console.h: include/console.h $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx/console.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx/console.h

$(PROJECT_INCLUDE)/mpc8xx/cpm.h: include/cpm.h $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx/cpm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx/cpm.h

$(PROJECT_INCLUDE)/mpc8xx/mmu.h: include/mmu.h $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx/mmu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx/mmu.h

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/raw_exception.h: exceptions/raw_exception.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/raw_exception.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/raw_exception.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
