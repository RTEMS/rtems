##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

EXTRA_DIST =
CLEANDIRS =
noinst_DATA =

include_libcpudir = $(includedir)/libcpu
include_libcpu_HEADERS = exceptions/raw_exception.h

# exceptions
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

# mmu
include_libcpu_HEADERS += mmu/bat.h mmu/pte121.h
mmu_C_FILES = mmu/bat.c mmu/pte121.c
mmu_S_FILES = mmu/mmuAsm.S
EXTRA_DIST += $(mmu_C_FILES) $(mmu_S_FILES)

mmu_rel_OBJECTS = $(mmu_C_FILES:mmu/%.c=mmu/$(ARCH)/%.$(OBJEXT)) \
    $(mmu_S_FILES:mmu/%.S=mmu/$(ARCH)/%.$(OBJEXT))

mmu/$(ARCH)/$(dirstamp):
	@$(mkdir_p) mmu/$(ARCH)
	@: > mmu/$(ARCH)/$(dirstamp)
mmu/$(ARCH)/%.$(OBJEXT): mmu/%.c mmu/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
mmu/$(ARCH)/%.$(OBJEXT): mmu/%.S mmu/$(ARCH)/$(dirstamp)
	${CCASCOMPILE} -o $@ -c $<
mmu/$(ARCH)/mmu.rel: $(mmu_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += mmu/o-optimize mmu/o-debug
noinst_DATA += mmu/$(ARCH)/mmu.rel

# clock
include_libcpu_HEADERS += clock/c_clock.h

clock_C_FILES = clock/c_clock.c
EXTRA_DIST += $(clock_C_FILES)

clock_rel_OBJECTS = $(clock_C_FILES:clock/%.c=clock/$(ARCH)/%.$(OBJEXT))

clock/$(ARCH)/$(dirstamp):
	@$(mkdir_p) clock/$(ARCH)
	@: > clock/$(ARCH)/$(dirstamp)
clock/$(ARCH)/%.$(OBJEXT): clock/%.c clock/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
clock/$(ARCH)/clock.rel: $(clock_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += clock/o-optimize clock/o-debug
noinst_DATA += clock/$(ARCH)/clock.rel

# timer

timer_C_FILES = timer/timer.c
EXTRA_DIST += $(timer_C_FILES)

timer_rel_OBJECTS = $(timer_C_FILES:timer/%.c=timer/$(ARCH)/%.$(OBJEXT))

timer/$(ARCH)/$(dirstamp):
	@$(mkdir_p) timer/$(ARCH)
	@: > timer/$(ARCH)/$(dirstamp)
timer/$(ARCH)/%.$(OBJEXT): timer/%.c timer/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
timer/$(ARCH)/timer.rel: $(timer_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += timer/o-optimize timer/o-debug
noinst_DATA += timer/$(ARCH)/timer.rel

## ----
all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/raw_exception.h: exceptions/raw_exception.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/raw_exception.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/raw_exception.h

$(PROJECT_INCLUDE)/libcpu/bat.h: mmu/bat.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/bat.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/bat.h

$(PROJECT_INCLUDE)/libcpu/pte121.h: mmu/pte121.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/pte121.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/pte121.h

$(PROJECT_INCLUDE)/libcpu/c_clock.h: clock/c_clock.h $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/libcpu/c_clock.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/libcpu/c_clock.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
