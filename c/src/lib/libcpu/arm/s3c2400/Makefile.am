##
## $Id$
##

noinst_PROGRAMS =

include $(top_srcdir)/../../../automake/compile.am

if s3c2400
include_s3c2400dir = $(includedir)/s3c2400
include_libcpudir = $(includedir)/libcpu
include_HEADERS = include/s3c2400.h

## clock
noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/clockdrv.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## timer
noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## interrupt
include_HEADERS += irq/irq.h

noinst_PROGRAMS += irq.rel
irq_rel_SOURCES = irq/irq.c irq/bsp_irq_init.c \
    ../../../libbsp/arm/shared/irq/irq_init.c irq/bsp_irq_asm.S \
    ../../../libbsp/arm/shared/irq/irq_asm.S irq/irq.h
irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

## --
all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

if s3c2400
$(PROJECT_INCLUDE)/s3c2400/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/s3c2400
	@: > $(PROJECT_INCLUDE)/s3c2400/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/s3c2400/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/s3c2400.h: include/s3c2400.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/s3c2400.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/s3c2400.h

$(PROJECT_INCLUDE)/irq.h: irq/irq.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/irq.h
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
