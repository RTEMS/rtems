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

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = ../../sparc/shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bsppredriverhook.c \
    ../../sparc/shared/bspstart.c ../../shared/bootcard.c \
    ../../shared/sbrk.c startup/setvec.c startup/spurious.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += gnatsupp.rel
gnatsupp_rel_SOURCES = gnatsupp/gnatsupp.c ../../sparc/shared/gnatcommon.c
gnatsupp_rel_CPPFLAGS = $(AM_CPPFLAGS)
gnatsupp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c console/consolereserveresources.c \
    console/debugputs.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# AMBA PnP Scanning
noinst_PROGRAMS += amba.rel
amba_rel_SOURCES = ../../sparc/shared/amba/ambapp.c
amba_rel_CPPFLAGS = $(AM_CPPFLAGS)
amba_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# PCI
noinst_PROGRAMS += pci.rel
pci_rel_SOURCES = pci/pci.c ../../sparc/shared/pci/pcifinddevice.c
pci_rel_CPPFLAGS = $(AM_CPPFLAGS)
pci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# RASTA Kit
noinst_PROGRAMS += rasta.rel
rasta_rel_SOURCES = rasta/rasta.c
rasta_rel_CPPFLAGS = $(AM_CPPFLAGS)
rasta_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# Companion Chip Kit
noinst_PROGRAMS += cchip.rel
cchip_rel_SOURCES = cchip/cchip.c
cchip_rel_CPPFLAGS = $(AM_CPPFLAGS)
cchip_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# B1553BRM
noinst_PROGRAMS += brm.rel
brm_rel_SOURCES = ../../sparc/shared/1553/b1553brm.c ../../sparc/shared/1553/b1553brm_pci.c ../../sparc/shared/1553/b1553brm_rasta.c
brm_rel_CPPFLAGS = $(AM_CPPFLAGS)
brm_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# CAN
noinst_PROGRAMS += can.rel
can_rel_SOURCES = ../../sparc/shared/can/occan.c ../../sparc/shared/can/occan_pci.c ../../sparc/shared/can/grcan.c ../../sparc/shared/can/grcan_rasta.c
can_rel_CPPFLAGS = $(AM_CPPFLAGS)
can_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# SpaceWire
noinst_PROGRAMS += spw.rel
spw_rel_SOURCES = ../../sparc/shared/spw/grspw.c ../../sparc/shared/spw/grspw_pci.c ../../sparc/shared/spw/grspw_rasta.c
spw_rel_CPPFLAGS = $(AM_CPPFLAGS)
spw_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# UART (RAW)
noinst_PROGRAMS += uart.rel
uart_rel_SOURCES = ../../sparc/shared/uart/apbuart.c ../../sparc/shared/uart/apbuart_pci.c ../../sparc/shared/uart/apbuart_rasta.c
uart_rel_CPPFLAGS = $(AM_CPPFLAGS)
uart_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# I2CMST
include_HEADERS += ../../sparc/shared/include/i2cmst.h
noinst_PROGRAMS += i2cmst.rel
i2cmst_rel_SOURCES = ../../sparc/shared/i2c/i2cmst.c
i2cmst_rel_CPPFLAGS = $(AM_CPPFLAGS)
i2cmst_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
noinst_PROGRAMS += leon_smc91111.rel
leon_smc91111_rel_SOURCES = leon_smc91111/leon_smc91111.c
leon_smc91111_rel_CPPFLAGS = $(AM_CPPFLAGS)
leon_smc91111_rel_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
leon_smc91111_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
noinst_PROGRAMS += leon_open_eth.rel
leon_open_eth_rel_SOURCES = leon_open_eth/leon_open_eth.c
leon_open_eth_rel_CPPFLAGS = $(AM_CPPFLAGS)
leon_open_eth_rel_CPPFLAGS +=  -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
leon_open_eth_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup.rel console.rel clock.rel timer.rel gnatsupp.rel \
                  amba.rel rasta.rel uart.rel spw.rel can.rel brm.rel pci.rel \
                  cchip.rel

if HAS_NETWORKING
libbsp_a_LIBADD += leon_open_eth.rel
libbsp_a_LIBADD += leon_smc91111.rel
endif

libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/reg_win.rel \
    ../../../libcpu/@RTEMS_CPU@/syscall.rel

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
