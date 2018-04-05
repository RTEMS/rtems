ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

if ENABLE_LCD
endif

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =


if ENABLE_LCD
endif

start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.csb337
dist_project_lib_DATA += startup/linkcmds.csb637


project_lib_LIBRARIES = librtemsbsp.a

# startup
librtemsbsp_a_SOURCES = \
    ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
    startup/bspstart.c \
    ../../shared/bspclean.c startup/bspreset.c \
    startup/memmap.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterread.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c
librtemsbsp_a_SOURCES += startup/pmc.c
librtemsbsp_a_SOURCES += clock/clock.c
librtemsbsp_a_SOURCES += timer/timer.c
# console
librtemsbsp_a_SOURCES += ../../shared/console.c console/uarts.c \
    ../../shared/console_select.c  ../../shared/console_control.c \
    ../../shared/console_read.c ../../shared/console_write.c 
librtemsbsp_a_SOURCES += console/dbgu.c
librtemsbsp_a_SOURCES += console/usart.c
# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += irq/irq.c
if ENABLE_LCD
librtemsbsp_a_SOURCES += console/sed1356.c console/fbcons.c
endif
# umon
if ENABLE_UMON
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/umon/monlib.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/umon/tfsDriver.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/umon/umonrtemsglue.c
endif
if ENABLE_UMON_CONSOLE
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/csb337/umon/umoncons.c
endif

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm920-mmu.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/csb337/headers.am
