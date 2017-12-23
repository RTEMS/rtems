##
#
# @file
#
# @brief Makefile of LibBSP for the BeagleBoards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  Data                                                       #
###############################################################################

EXTRA_DIST += ../shared/start/start.S
start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.beagle

###############################################################################
#                  LibBSP                                                     #
###############################################################################

noinst_LIBRARIES = libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS = $(AM_CPPFLAGS)
libbsp_a_LIBADD =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/src/bsp-fdt.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../../shared/gpio.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/timerstub.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
libbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c

# Startup
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/bspstarthooks.c
libbsp_a_SOURCES += startup/bspstartmmu.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq.c

# Console
libbsp_a_SOURCES += ../../shared/console.c \
        ../../shared/console_select.c \
  console/console-config.c \
        ../../shared/console_read.c \
        ../../shared/console_write.c \
        ../../shared/console_control.c

# I2C
libbsp_a_SOURCES += i2c/bbb-i2c.c

# GPIO
libbsp_a_SOURCES += gpio/bbb-gpio.c

#pwm
libbsp_a_SOURCES += pwm/pwm.c

#RTC
libbsp_a_SOURCES += rtc.c
libbsp_a_SOURCES += ../../shared/tod.c
# Clock
libbsp_a_SOURCES += clock.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../shared/include/arm-cache-l1.h
libbsp_a_SOURCES += ../shared/armv467ar-basic-cache/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/armv467ar-basic-cache

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/beagle/headers.am
