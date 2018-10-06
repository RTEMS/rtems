ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/csb337/start/bsp_specs

if ENABLE_LCD
endif

noinst_PROGRAMS =


if ENABLE_LCD
endif

start.$(OBJEXT): ../../../../../../bsps/arm/csb337/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/arm/csb337/start/linkcmds.csb337
dist_project_lib_DATA += ../../../../../../bsps/arm/csb337/start/linkcmds.csb637


project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/start/memmap.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/cpucounter/cpucounterfrequency.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/cpucounter/cpucounterread.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/start/pmc.c
librtemsbsp_a_SOURCES +=../../../../../../bsps/arm/csb337/clock/clock.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/btimer/btimer.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/console/uarts.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/console/dbgu.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/console/usart.c
# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/irq/irq.c
if ENABLE_LCD
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/console/sed1356.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/console/fbcons.c
endif
# umon
if ENABLE_UMON
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/umon/monlib.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/umon/tfsDriver.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/umon/umonrtemsglue.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/start/umonsupp.c
endif
if ENABLE_UMON_CONSOLE
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/umon/umoncons.c
endif

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/net/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm920-mmu.c

include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/csb337/headers.am
