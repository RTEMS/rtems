##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

dist_noinst_SCRIPTS = coff2bin elf2exe

EXTRA_DIST = README

EXTRA_DIST += debug_ada/Makefile.ts_386ex debug_ada/cmds debug_ada/ddd-rtems \
    debug_ada/debug.adb debug_ada/init.c debug_ada/serial_debug.adb \
    debug_ada/serial_debug.ads

EXTRA_DIST += debug_c/cmds debug_c/ddd-rtems debug_c/debug_test.c \
    debug_c/init.c debug_c/serial_gdb.c debug_c/serial_gdb.h \
    debug_c/system.h

EXTRA_DIST += dos_sup/loader.com dos_sup/loader_hybrid_com.asm \
    dos_sup/ts1325.inc

EXTRA_DIST += network_ada/adasockets/AUTHORS network_ada/adasockets/COPYING \
    network_ada/adasockets/NEWS network_ada/adasockets/README \
    network_ada/adasockets/sockets-constants.ads \
    network_ada/adasockets/sockets-link.ads \
    network_ada/adasockets/sockets-multicast.adb \
    network_ada/adasockets/sockets-multicast.ads \
    network_ada/adasockets/sockets-naming.adb \
    network_ada/adasockets/sockets-naming.ads \
    network_ada/adasockets/sockets-thin.ads \
    network_ada/adasockets/sockets-utils.adb \
    network_ada/adasockets/sockets-utils.ads \
    network_ada/adasockets/sockets.adb network_ada/adasockets/sockets.ads

EXTRA_DIST += network_ada/README network_ada/networkconfig.h

EXTRA_DIST += network_ada/listener/Makefile.ts_386ex \
    network_ada/listener/init.c network_ada/listener/listener.adb

EXTRA_DIST += network_ada/tcprelay/Makefile.ts_386ex \
    network_ada/tcprelay/init.c network_ada/tcprelay/print_error.c \
    network_ada/tcprelay/tcprelay.adb

EXTRA_DIST += ts_1325_ada/Makefile.ts_386ex ts_1325_ada/README \
    ts_1325_ada/i386_ports.adb ts_1325_ada/i386_ports.ads ts_1325_ada/init.c \
    ts_1325_ada/ts1325-button.adb ts_1325_ada/ts1325-button.ads \
    ts_1325_ada/ts1325-led.adb ts_1325_ada/ts1325-led.ads \
    ts_1325_ada/ts1325-parallel.adb ts_1325_ada/ts1325-parallel.ads \
    ts_1325_ada/ts1325.ads ts_1325_ada/ts1325_test.adb

include $(top_srcdir)/../../../../../automake/host.am
