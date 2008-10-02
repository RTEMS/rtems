##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

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
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools

include_HEADERS += include/leon.h
include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = ../../sparc/shared/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bsppredriverhook.c \
    ../../sparc/shared/bspgetworkarea.c ../../sparc/shared/bspstart.c \
    ../../shared/bootcard.c ../../shared/sbrk.c startup/setvec.c \
    startup/spurious.c
# gnatsupp
libbsp_a_SOURCES += gnatsupp/gnatsupp.c ../../sparc/shared/gnatcommon.c
# console
libbsp_a_SOURCES += console/console.c console/debugputs.c
# clock
libbsp_a_SOURCES += clock/ckinit.c ../../../shared/clockdrv_shell.h
# AMBA PnP Scanning
libbsp_a_SOURCES += ../../sparc/shared/amba/ambapp.c
# PCI
libbsp_a_SOURCES += pci/pci.c ../../sparc/shared/pci/pcifinddevice.c
# RASTA Kit
libbsp_a_SOURCES += rasta/rasta.c
# Companion Chip Kit
libbsp_a_SOURCES += cchip/cchip.c
# B1553BRM
libbsp_a_SOURCES += ../../sparc/shared/1553/b1553brm.c \
    ../../sparc/shared/1553/b1553brm_pci.c \
    ../../sparc/shared/1553/b1553brm_rasta.c
# CAN
libbsp_a_SOURCES += ../../sparc/shared/can/occan.c \
    ../../sparc/shared/can/occan_pci.c \
    ../../sparc/shared/can/grcan.c \
    ../../sparc/shared/can/grcan_rasta.c
# SpaceWire
libbsp_a_SOURCES += ../../sparc/shared/spw/grspw.c \
    ../../sparc/shared/spw/grspw_pci.c ../../sparc/shared/spw/grspw_rasta.c
# UART (RAW)
libbsp_a_SOURCES += ../../sparc/shared/uart/apbuart.c \
    ../../sparc/shared/uart/apbuart_pci.c \
    ../../sparc/shared/uart/apbuart_rasta.c
# I2CMST
include_HEADERS += ../../sparc/shared/include/i2cmst.h
libbsp_a_SOURCES += ../../sparc/shared/i2c/i2cmst.c

# timer
libbsp_a_SOURCES += timer/timer.c

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

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/reg_win.rel \
    ../../../libcpu/@RTEMS_CPU@/syscall.rel

if HAS_NETWORKING
libbsp_a_LIBADD += leon_open_eth.rel
libbsp_a_LIBADD += leon_smc91111.rel
endif

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
