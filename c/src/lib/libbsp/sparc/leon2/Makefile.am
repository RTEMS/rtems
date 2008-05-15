##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h
include_HEADERS += include/rasta.h
include_HEADERS += include/cchip.h
include_HEADERS += ../../sparc/shared/include/ambapp.h
include_HEADERS += ../../sparc/shared/include/grspw.h
include_HEADERS += ../../sparc/shared/include/grspw_pci.h
include_HEADERS += ../../sparc/shared/include/grspw_rasta.h
include_HEADERS += ../../sparc/shared/include/occan.h
include_HEADERS += ../../sparc/shared/include/occan_pci.h
include_HEADERS += ../../sparc/shared/include/grcan.h
include_HEADERS += ../../sparc/shared/include/grcan_rasta.h
include_HEADERS += ../../sparc/shared/include/apbuart.h
include_HEADERS += ../../sparc/shared/include/apbuart_pci.h
include_HEADERS += ../../sparc/shared/include/apbuart_rasta.h
include_HEADERS += ../../sparc/shared/include/b1553brm.h
include_HEADERS += ../../sparc/shared/include/b1553brm_pci.h
include_HEADERS += ../../sparc/shared/include/b1553brm_rasta.h
include_HEADERS += ../../sparc/shared/include/debug_defs.h
include_HEADERS += ../../sparc/shared/include/pci.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools

include_HEADERS += include/leon.h
include_HEADERS += include/coverhd.h

EXTRA_DIST = ../../sparc/shared/start.S
start.$(OBJEXT): ../../sparc/shared/start.S
	$(CPPASCOMPILE) -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = ../../sparc/shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bsppredriverhook.c \
    ../../sparc/shared/bspstart.c ../../shared/bootcard.c \
    ../../shared/sbrk.c startup/setvec.c startup/spurious.c
gnatsupp_SOURCES = gnatsupp/gnatsupp.c ../../sparc/shared/gnatcommon.c
console_SOURCES = console/console.c console/debugputs.c
clock_SOURCES = clock/ckinit.c
# AMBA PnP Scanning
amba_SOURCES = ../../sparc/shared/amba/ambapp.c
# PCI
pci_SOURCES = pci/pci.c ../../sparc/shared/pci/pcifinddevice.c
# RASTA Kit
rasta_SOURCES = rasta/rasta.c
# Companion Chip Kit
cchip_SOURCES = cchip/cchip.c
# B1553BRM
brm_SOURCES = ../../sparc/shared/1553/b1553brm.c \
    ../../sparc/shared/1553/b1553brm_pci.c \
    ../../sparc/shared/1553/b1553brm_rasta.c
# CAN
can_SOURCES = ../../sparc/shared/can/occan.c \
    ../../sparc/shared/can/occan_pci.c \
    ../../sparc/shared/can/grcan.c \
    ../../sparc/shared/can/grcan_rasta.c
# SpaceWire
spw_SOURCES = ../../sparc/shared/spw/grspw.c \
    ../../sparc/shared/spw/grspw_pci.c ../../sparc/shared/spw/grspw_rasta.c
# UART (RAW)
uart_SOURCES = ../../sparc/shared/uart/apbuart.c \
    ../../sparc/shared/uart/apbuart_pci.c \
    ../../sparc/shared/uart/apbuart_rasta.c
# I2CMST
include_HEADERS += ../../sparc/shared/include/i2cmst.h
i2cmst_SOURCES = ../../sparc/shared/i2c/i2cmst.c

timer_SOURCES = timer/timer.c

if HAS_NETWORKING
noinst_PROGRAMS += leon_smc91111.rel
leon_smc91111_rel_SOURCES = leon_smc91111/leon_smc91111.c
leon_smc91111_rel_CPPFLAGS = $(AM_CPPFLAGS)
leon_smc91111_rel_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
leon_smc91111_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if HAS_NETWORKING
noinst_PROGRAMS += leon_open_eth.rel
leon_open_eth_rel_SOURCES = leon_open_eth/leon_open_eth.c
leon_open_eth_rel_CPPFLAGS = $(AM_CPPFLAGS)
leon_open_eth_rel_CPPFLAGS +=  -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
leon_open_eth_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(gnatsupp_SOURCES) $(console_SOURCES) \
    $(clock_SOURCES) $(amba_SOURCES) $(pci_SOURCES) $(rasta_SOURCES) \
    $(cchip_SOURCES) $(brm_SOURCES) $(can_SOURCES) $(spw_SOURCES) \
    $(uart_SOURCES) $(i2cmst_SOURCES) $(timer_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/reg_win.rel \
    ../../../libcpu/@RTEMS_CPU@/syscall.rel

if HAS_NETWORKING
libbsp_a_LIBADD += leon_open_eth.rel
libbsp_a_LIBADD += leon_smc91111.rel
endif

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
