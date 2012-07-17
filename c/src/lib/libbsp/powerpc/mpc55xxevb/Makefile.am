ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = startup/start.S
project_lib_DATA = start.$(OBJEXT)

libbspstart_a_SOURCES += ../shared/start/rtems_crti.S
project_lib_DATA += rtems_crti.$(OBJEXT)

# Link commands
project_lib_DATA += startup/linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base	
dist_project_lib_DATA += startup/linkcmds.mpc55xx
dist_project_lib_DATA += startup/linkcmds.gwlcfm
dist_project_lib_DATA += startup/linkcmds.mpc5566evb
dist_project_lib_DATA += startup/linkcmds.mpc5566evb_spe
dist_project_lib_DATA += startup/linkcmds.mpc5643l_evb
dist_project_lib_DATA += startup/linkcmds.mpc5643l_dpu
dist_project_lib_DATA += startup/linkcmds.mpc5674fevb
dist_project_lib_DATA += startup/linkcmds.mpc5674fevb_spe
dist_project_lib_DATA += startup/linkcmds.phycore_mpc5554
dist_project_lib_DATA += startup/linkcmds.mpc5674f_ecu508_boot
dist_project_lib_DATA += startup/linkcmds.mpc5674f_ecu508_app

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# Includes
include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h ../../shared/tod.h \
    ../../shared/include/coverhd.h
include_bsp_HEADERS =
include_bsp_HEADERS += ../../../libcpu/powerpc/mpc83xx/i2c/mpc83xx_i2cdrv.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../shared/include/linker-symbols.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += ../shared/include/tictac.h
include_bsp_HEADERS += include/mpc55xx-config.h
include_bsp_HEADERS += include/mpc55xxevb.h
include_bsp_HEADERS += include/smsc9218i.h
include_bsp_HEADERS += include/console-esci.h
include_bsp_HEADERS += include/console-generic.h
include_bsp_HEADERS += include/console-linflex.h

# startup
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bsplibc.c
libbsp_a_SOURCES += ../../shared/bsppost.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../shared/src/bsp-start-zero.S
libbsp_a_SOURCES += ../shared/src/memcpy.c
libbsp_a_SOURCES += ../shared/src/tictac.c
libbsp_a_SOURCES += ../shared/startup/bspidle.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/exc-vector-base.S
libbsp_a_SOURCES += startup/get-system-clock.c
libbsp_a_SOURCES += startup/reset.c
libbsp_a_SOURCES += startup/restart.c
libbsp_a_SOURCES += startup/start-config-clock.c
libbsp_a_SOURCES += startup/start-config-ebi-cs.c
libbsp_a_SOURCES += startup/start-config-ebi-cs-cal.c
libbsp_a_SOURCES += startup/start-config-mmu.c
libbsp_a_SOURCES += startup/start-config-mmu-early.c
libbsp_a_SOURCES += startup/start-config-siu-pcr.c
libbsp_a_SOURCES += startup/start-early.c
libbsp_a_SOURCES += startup/start-cache.S
libbsp_a_SOURCES += startup/start-clock.c
libbsp_a_SOURCES += startup/start-flash.S
libbsp_a_SOURCES += startup/start-watchdog.c

# clock
libbsp_a_SOURCES += clock/clock-config.c

# console
libbsp_a_SOURCES += console/console-config.c
libbsp_a_SOURCES += console/console-esci.c
libbsp_a_SOURCES += console/console-generic.c
libbsp_a_SOURCES += console/console-linflex.c

# irq_generic
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c

# Misc
libbsp_a_SOURCES += startup/sd-card-init.c

# I2C
libbsp_a_SOURCES += i2c/i2c_init.c \
    ../../../libcpu/powerpc/mpc83xx/i2c/mpc83xx_i2cdrv.c

# Network
if HAS_NETWORKING
libbsp_a_SOURCES += network/smsc9218i.c
libbsp_a_SOURCES += network/if_smc.c
endif

# BSP library

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/misc.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/irq.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/siu.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/edma.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/emios.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/dspi.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
