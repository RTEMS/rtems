##
## $Id$
##

CLEANDIRS =
EXTRA_DIST =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include_HEADERS = ictrl/ictrl.h

# ictrl
ictrl_C_FILES = ictrl/ictrl.c
EXTRA_DIST += $(ictrl_C_FILES)

ictrl_rel_OBJECTS = $(ictrl_C_FILES:ictrl/%.c=icrtl/$(ARCH)/%.$(OBJEXT))

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

## vector
vectors_S_FILES = vectors.S
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

include $(top_srcdir)/../../../automake/local.am
