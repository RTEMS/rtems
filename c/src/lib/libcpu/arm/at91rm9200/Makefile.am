##
## $Id$
##

CLEANFILES =
EXTRA_DIST =
EXTRA_PROGRAMS =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am

if at91rm9200
include_at91rm9200dir = $(includedir)/at91rm9200
include_libcpudir = $(includedir)/libcpu
include_HEADERS =
include_HEADERS += include/at91rm9200.h include/at91rm9200_dbgu.h \
    include/at91rm9200_emac.h include/at91rm9200_gpio.h \
    include/at91rm9200_mem.h include/at91rm9200_pmc.h include/bits.h

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

## dbgu
EXTRA_PROGRAMS += dbgu.rel
CLEANFILES += dbgu.rel
dbgu_rel_SOURCES = dbgu/dbgu.c
dbgu_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
dbgu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += dbgu_g.rel
CLEANFILES += dbgu_g.rel
dbgu_g_rel_SOURCES = $(dbgu_rel_SOURCES)
dbgu_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
dbgu_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += dbgu$(LIB_VARIANT).rel

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

## pmc
EXTRA_PROGRAMS += pmc.rel
CLEANFILES += pmc.rel
pmc_rel_SOURCES = pmc/pmc.c
pmc_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
pmc_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += pmc_g.rel
CLEANFILES += pmc_g.rel
pmc_g_rel_SOURCES = $(pmc_rel_SOURCES)
pmc_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
pmc_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += pmc$(LIB_VARIANT).rel

## interrupt
include_HEADERS += irq/irq.h

EXTRA_PROGRAMS += irq.rel
CLEANFILES += irq.rel
irq_rel_SOURCES = irq/irq.c irq/bsp_irq_init.c \
    ../../../libbsp/arm/shared/irq/irq_init.c irq/bsp_irq_asm.S \
    ../../../libbsp/arm/shared/irq/irq_asm.S irq/irq.h
irq_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += irq_g.rel
CLEANFILES += irq_g.rel
irq_g_rel_SOURCES = $(irq_rel_SOURCES)
irq_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
irq_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += irq$(LIB_VARIANT).rel
endif

## --
all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

if at91rm9200
$(PROJECT_INCLUDE)/at91rm9200/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/at91rm9200
	@: > $(PROJECT_INCLUDE)/at91rm9200/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/at91rm9200/$(dirstamp)

$(PROJECT_INCLUDE)/libcpu/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/libcpu
	@: > $(PROJECT_INCLUDE)/libcpu/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/libcpu/$(dirstamp)

$(PROJECT_INCLUDE)/at91rm9200.h: include/at91rm9200.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/at91rm9200.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/at91rm9200.h

$(PROJECT_INCLUDE)/at91rm9200_dbgu.h: include/at91rm9200_dbgu.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/at91rm9200_dbgu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/at91rm9200_dbgu.h

$(PROJECT_INCLUDE)/at91rm9200_emac.h: include/at91rm9200_emac.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/at91rm9200_emac.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/at91rm9200_emac.h

$(PROJECT_INCLUDE)/at91rm9200_gpio.h: include/at91rm9200_gpio.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/at91rm9200_gpio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/at91rm9200_gpio.h

$(PROJECT_INCLUDE)/at91rm9200_mem.h: include/at91rm9200_mem.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/at91rm9200_mem.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/at91rm9200_mem.h

$(PROJECT_INCLUDE)/at91rm9200_pmc.h: include/at91rm9200_pmc.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/at91rm9200_pmc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/at91rm9200_pmc.h

$(PROJECT_INCLUDE)/bits.h: include/bits.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bits.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bits.h

$(PROJECT_INCLUDE)/irq.h: irq/irq.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/irq.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
