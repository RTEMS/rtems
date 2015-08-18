##
#
# @file
#
# @brief Makefile of LibBSP for the BeagleBoards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp
include_libcpudir = $(includedir)/libcpu

dist_project_lib_DATA = bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = ../../shared/include/coverhd.h \
  include/bspopts.h

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../../shared/include/uart-output-char.h
include_bsp_HEADERS += ../shared/include/arm-a8core-start.h
include_bsp_HEADERS += ../shared/include/arm-cp15-start.h
include_bsp_HEADERS += ../shared/include/arm-errata.h
include_bsp_HEADERS += ../shared/include/arm-release-id.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/i2c.h
include_bsp_HEADERS += include/beagleboneblack.h
include_bsp_HEADERS += include/bbb-gpio.h

include_libcpu_HEADERS =
include_libcpu_HEADERS += ../../../libcpu/arm/shared/include/arm-cp15.h
include_libcpu_HEADERS += ../../../libcpu/arm/shared/include/omap3.h
include_libcpu_HEADERS += ../../../libcpu/arm/shared/include/am335x.h
include_libcpu_HEADERS += ../../../libcpu/arm/shared/include/omap_timer.h

###############################################################################
#                  Data                                                       #
###############################################################################

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += startup/linkcmds
project_lib_DATA += startup/linkcmds.beagle

EXTRA_DIST = startup/linkcmds.beagle

###############################################################################
#                  LibBSP                                                     #
###############################################################################

noinst_LIBRARIES += libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS =
libbsp_a_LIBADD =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsplibc.c
libbsp_a_SOURCES += ../../shared/bsppost.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/bsppretaskinghook.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../../shared/gpio.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/timerstub.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c

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
libbsp_a_SOURCES += misc/i2c.c

# GPIO
libbsp_a_SOURCES += gpio/bbb-gpio.c

#RTC
libbsp_a_SOURCES += rtc.c
libbsp_a_SOURCES += ../../shared/tod.c
# Clock
libbsp_a_SOURCES += clock.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../../libcpu/arm/shared/include/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../../../libcpu/arm/shared/include

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
