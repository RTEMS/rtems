##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

# include
include_mcf5206dir = $(includedir)/mcf5206
include_mcf5206_HEADERS = include/mcf5206e.h include/mcfmbus.h \
    include/mcfuart.h

## clock

clock_C_FILES = clock/ckinit.c
clock_OBJS = $(clock_C_FILES:clock/%.c=clock/$(ARCH)/%.$(OBJEXT))
EXTRA_DIST = $(clock_C_FILES)

clock/$(ARCH)/$(dirstamp):
	@$(mkdir_p) clock/$(ARCH)
	@: > clock/$(ARCH)/$(dirstamp)
clock/$(ARCH)/%.$(OBJEXT): clock/%.c clock/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
clock/$(ARCH)/clock.rel: $(clock_OBJS)
	$(make-rel)
PGMS = clock/$(ARCH)/clock.rel

## console

console_C_FILES = console/mcfuart.c
console_OBJS = $(console_C_FILES:console/%.c=console/$(ARCH)/%.$(OBJEXT))
EXTRA_DIST += $(console_C_FILES)

console/$(ARCH)/$(dirstamp):
	@$(mkdir_p) console/$(ARCH)
	@: > console/$(ARCH)/$(dirstamp)
console/$(ARCH)/%.$(OBJEXT): console/%.c console/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
console/$(ARCH)/console.rel: $(console_OBJS)
	$(make-rel)
PGMS += console/$(ARCH)/console.rel

## mbus

mbus_C_FILES = mbus/mcfmbus.c
mbus_OBJS = $(mbus_C_FILES:mbus/%.c=mbus/$(ARCH)/%.$(OBJEXT))
EXTRA_DIST += $(mbus_C_FILES)

mbus/$(ARCH)/$(dirstamp):
	@$(mkdir_p) mbus/$(ARCH)
	@: > mbus/$(ARCH)/$(dirstamp)
mbus/$(ARCH)/%.$(OBJEXT): mbus/%.c mbus/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
mbus/$(ARCH)/mbus.rel: $(mbus_OBJS)
	$(make-rel)
PGMS += mbus/$(ARCH)/mbus.rel

## timer

timer_C_FILES = timer/timer.c
timer_OBJS = $(timer_C_FILES:timer/%.c=timer/$(ARCH)/%.$(OBJEXT))
EXTRA_DIST += $(timer_C_FILES)

timer_S_FILES = timer/timerisr.S
timer_OBJS += $(timer_S_FILES:timer/%.S=timer/$(ARCH)/%.$(OBJEXT))
EXTRA_DIST += $(timer_S_FILES)

timer/$(ARCH)/$(dirstamp):
	@$(mkdir_p) timer/$(ARCH)
	@: > timer/$(ARCH)/$(dirstamp)
timer/$(ARCH)/%.$(OBJEXT): timer/%.c timer/$(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
timer/$(ARCH)/%.o: timer/%.S timer/$(ARCH)/$(dirstamp)
	${CCASCOMPILE} -DASM -o $@ -c $<
timer/$(ARCH)/timer.rel: $(timer_OBJS)
	$(make-rel)
PGMS += timer/$(ARCH)/timer.rel

## --

all-local: $(PGMS)
CLEANDIRS = */$(ARCH)

.PRECIOUS: $(PGMS)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/mcf5206/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/mcf5206
	@: > $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)

$(PROJECT_INCLUDE)/mcf5206/mcf5206e.h: include/mcf5206e.h $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mcf5206/mcf5206e.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5206/mcf5206e.h

$(PROJECT_INCLUDE)/mcf5206/mcfmbus.h: include/mcfmbus.h $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mcf5206/mcfmbus.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5206/mcfmbus.h

$(PROJECT_INCLUDE)/mcf5206/mcfuart.h: include/mcfuart.h $(PROJECT_INCLUDE)/mcf5206/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mcf5206/mcfuart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mcf5206/mcfuart.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
