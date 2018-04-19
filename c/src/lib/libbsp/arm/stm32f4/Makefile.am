ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

###############################################################################
#                  Data                                                       #
###############################################################################

dist_project_lib_DATA = startup/bsp_specs

start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.stm32f105rc
dist_project_lib_DATA += startup/linkcmds.stm32f4

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/cpucounter/cpucounterread.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/cpucounter/cpucounterdiff.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c

# Startup
librtemsbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/bspstarthook.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/io.c
librtemsbsp_a_SOURCES += startup/rcc.c
librtemsbsp_a_SOURCES += startup/start-config-io.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq.c
librtemsbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq-dispatch.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c
librtemsbsp_a_SOURCES += console/console-config.c
librtemsbsp_a_SOURCES += console/usart.c

# I2C
librtemsbsp_a_SOURCES += i2c/i2c.c
librtemsbsp_a_SOURCES += i2c/i2c-config.c

# Clock
librtemsbsp_a_SOURCES += ../shared/armv7m/clock/armv7m-clock-config.c

# Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/stm32f4/headers.am
