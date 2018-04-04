##
#
# @file
#
# @brief Makefile of LibBSP for the BeagleBoards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  Data                                                       #
###############################################################################

start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.beagle

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =
librtemsbsp_a_LIBADD =

# Shared
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/src/bsp-fdt.c
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/src/stackalloc.c
librtemsbsp_a_SOURCES += ../../shared/gpio.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c
librtemsbsp_a_SOURCES += ../../shared/timerstub.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterread.c
librtemsbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
librtemsbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/bspstarthooks.c
librtemsbsp_a_SOURCES += startup/bspstartmmu.c

# IRQ
librtemsbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-generic.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-info.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-legacy.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-server.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-shell.c
librtemsbsp_a_SOURCES += irq.c

# Console
librtemsbsp_a_SOURCES += ../../shared/console.c \
        ../../shared/console_select.c \
  console/console-config.c \
        ../../shared/console_read.c \
        ../../shared/console_write.c \
        ../../shared/console_control.c

# I2C
librtemsbsp_a_SOURCES += i2c/bbb-i2c.c

# GPIO
librtemsbsp_a_SOURCES += gpio/bbb-gpio.c

#pwm
librtemsbsp_a_SOURCES += pwm/pwm.c

#RTC
librtemsbsp_a_SOURCES += rtc.c
librtemsbsp_a_SOURCES += ../../shared/tod.c
# Clock
librtemsbsp_a_SOURCES += clock.c
librtemsbsp_a_SOURCES += ../../shared/clockdrv_shell.h

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/beagle/headers.am
