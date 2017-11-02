ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp
include_libcpudir = $(includedir)/libcpu
include_arm_freescale_imxdir = $(includedir)/arm/freescale/imx

dist_project_lib_DATA = bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = ../../shared/include/coverhd.h \
	include/bspopts.h

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += ../shared/include/arm-a9mpcore-regs.h
include_bsp_HEADERS += ../shared/include/arm-a9mpcore-start.h
include_bsp_HEADERS += ../shared/include/arm-cp15-start.h
include_bsp_HEADERS += ../shared/include/arm-errata.h
include_bsp_HEADERS += ../shared/include/arm-gic.h
include_bsp_HEADERS += ../shared/include/arm-gic-irq.h
include_bsp_HEADERS += ../shared/include/arm-gic-regs.h
include_bsp_HEADERS += ../shared/include/arm-gic-tm27.h
include_bsp_HEADERS += ../shared/include/arm-release-id.h
include_bsp_HEADERS += include/irq.h

include_libcpu_HEADERS = ../../../libcpu/arm/shared/include/arm-cp15.h

include_arm_freescale_imx_HEADERS =
include_arm_freescale_imx_HEADERS += include/arm/freescale/imx/imx_ccmvar.h
include_arm_freescale_imx_HEADERS += include/arm/freescale/imx/imx_ecspireg.h
include_arm_freescale_imx_HEADERS += include/arm/freescale/imx/imx_gpcreg.h
include_arm_freescale_imx_HEADERS += include/arm/freescale/imx/imx_i2creg.h
include_arm_freescale_imx_HEADERS += include/arm/freescale/imx/imx_iomuxreg.h
include_arm_freescale_imx_HEADERS += include/arm/freescale/imx/imx_iomuxvar.h
include_arm_freescale_imx_HEADERS += include/arm/freescale/imx/imx_srcreg.h
include_arm_freescale_imx_HEADERS += include/arm/freescale/imx/imx_uartreg.h
include_arm_freescale_imx_HEADERS += include/arm/freescale/imx/imx_wdogreg.h

###############################################################################
#                  Data                                                       #
###############################################################################

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += startup/linkcmds
project_lib_DATA += startup/linkcmds.imx7

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
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/timerstub.c
libbsp_a_SOURCES += ../../shared/src/bsp-fdt.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
libbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c

# Startup
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/ccm.c
libbsp_a_SOURCES += startup/imx_iomux.c
if HAS_SMP
libbsp_a_SOURCES += ../shared/arm-a9mpcore-smp.c
libbsp_a_SOURCES += startup/bspsmp.c
endif

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += ../shared/arm-gic-irq.c

# Console
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/console-config.c

# Clock
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
libbsp_a_SOURCES += ../shared/arm-generic-timer-clock-config.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../shared/include/arm-cache-l1.h
libbsp_a_SOURCES += ../shared/armv467ar-basic-cache/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/armv467ar-basic-cache

# I2C
libbsp_a_SOURCES += i2c/imx-i2c.c

# SPI
libbsp_a_SOURCES += spi/imx-ecspi.c

# Start hooks
libbsp_a_SOURCES += startup/bspstarthooks.c

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
