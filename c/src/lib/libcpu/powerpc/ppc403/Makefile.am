##
## $Id$
##

CLEANDIRS =
EXTRA_DIST =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

## clock
clock_C_FILES = clock/clock.c
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

## console
if ppc403
console_C_FILES = console/console.c
endif
EXTRA_DIST += console/console.c
if ppc405
console_C_FILES = console/console405.c
endif
EXTRA_DIST += console/console405.c

console_rel_OBJECTS = \
    $(console_C_FILES:console/%.c=console/$(ARCH)/%.$(OBJEXT))
console/$(ARCH)/$(dirstamp):
	@$(mkdir_p) console/$(ARCH)
	@: > console/$(ARCH)/$(dirstamp)
console/$(ARCH)/%.$(OBJEXT): console/%.c console/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
console/$(ARCH)/console.rel: $(console_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += console/o-optimize console/o-debug
noinst_DATA += console/$(ARCH)/console.rel

## ictrl
include_HEADERS = ictrl/ictrl.h
ictrl_C_FILES = ictrl/ictrl.c
EXTRA_DIST += $(ictrl_C_FILES)

ictrl_rel_OBJECTS = $(ictrl_C_FILES:ictrl/%.c=ictrl/$(ARCH)/%.$(OBJEXT))
ictrl/$(ARCH)/$(dirstamp):
	@$(mkdir_p) ictrl/$(ARCH)
	@: > ictrl/$(ARCH)/$(dirstamp)
ictrl/$(ARCH)/%.$(OBJEXT): ictrl/%.c ictrl/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
ictrl/$(ARCH)/ictrl.rel: $(ictrl_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += ictrl/o-optimize ictrl/o-debug
noinst_DATA += ictrl/$(ARCH)/ictrl.rel

## timer
timer_C_FILES = timer/timer.c

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

## tty_drv
if ppc405
include_HEADERS += tty_drv/tty_drv.h
tty_drv_C_FILES = tty_drv/tty_drv.c
EXTRA_DIST += $(tty_drv_C_FILES)

tty_drv_rel_OBJECTS = \
    $(tty_drv_C_FILES:tty_drv/%.c=tty_drv/$(ARCH)/%.$(OBJEXT))

tty_drv/$(ARCH)/$(dirstamp):
	@$(mkdir_p) tty_drv/$(ARCH)
	@: > tty_drv/$(ARCH)/$(dirstamp)
tty_drv/$(ARCH)/%.$(OBJEXT): tty_drv/%.c tty_drv/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
tty_drv/$(ARCH)/tty_drv.rel: $(tty_drv_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += tty_drv/o-optimize tty_drv/o-debug
noinst_DATA += tty_drv/$(ARCH)/tty_drv.rel
endif

## vectors
vectors_S_FILES = vectors/vectors.S vectors/align_h.S
EXTRA_DIST += $(vectors_S_FILES)

vectors_rel_OBJECTS = \
    $(vectors_S_FILES:vectors/%.S=vectors/${ARCH}/%.$(OBJEXT))

vectors/$(ARCH)/$(dirstamp):
	@$(mkdir_p) vectors/$(ARCH)
	@: > vectors/$(ARCH)/$(dirstamp)
vectors/$(ARCH)/%.$(OBJEXT): vectors/%.S vectors/$(ARCH)/$(dirstamp)
	${CCASCOMPILE} -o $@ -c $<
vectors/$(ARCH)/vectors.rel: $(vectors_rel_OBJECTS)
	$(make-rel)
CLEANDIRS += vectors/o-optimize vectors/o-debug
noinst_DATA += vectors/$(ARCH)/vectors.rel

## --
all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/ictrl.h: ictrl/ictrl.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ictrl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ictrl.h

if ppc405
$(PROJECT_INCLUDE)/tty_drv.h: tty_drv/tty_drv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tty_drv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tty_drv.h
endif

include $(top_srcdir)/../../../automake/local.am
