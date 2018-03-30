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

noinst_LIBRARIES = libbsp.a

libbsp_a_SOURCES =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c

# Startup
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/bspstarthook.c
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += startup/io.c
libbsp_a_SOURCES += startup/rcc.c
libbsp_a_SOURCES += startup/start-config-io.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq.c
libbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq-dispatch.c

# Console
libbsp_a_SOURCES += ../../shared/console.c
libbsp_a_SOURCES += ../../shared/console_control.c
libbsp_a_SOURCES += ../../shared/console_read.c
libbsp_a_SOURCES += ../../shared/console_select.c
libbsp_a_SOURCES += ../../shared/console_write.c
libbsp_a_SOURCES += console/console-config.c
libbsp_a_SOURCES += console/usart.c

# I2C
libbsp_a_SOURCES += i2c/i2c.c
libbsp_a_SOURCES += i2c/i2c-config.c

# Clock
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
libbsp_a_SOURCES += ../shared/armv7m/clock/armv7m-clock-config.c

# Timer
libbsp_a_SOURCES += ../../shared/timerstub.c

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/stm32f4/headers.am
