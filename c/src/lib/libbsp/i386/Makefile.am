ACLOCAL_AMFLAGS = -I ../../../aclocal

## Descend into the @RTEMS_BSP_FAMILY@ directory
SUBDIRS = @RTEMS_BSP_FAMILY@

EXTRA_DIST =

# shared/comm
EXTRA_DIST += shared/comm/uart.c shared/comm/uart.h
EXTRA_DIST += shared/comm/tty_drv.c shared/comm/tty_drv.h
EXTRA_DIST += shared/comm/i386_io.h
EXTRA_DIST += shared/comm/i386-stub-glue.c shared/comm/i386-stub.c
EXTRA_DIST += shared/comm/gdb_glue.c

EXTRA_DIST += shared/comm/GDB.HOWTO

# shared/irq
EXTRA_DIST += shared/irq/irq.h shared/irq/irq.c
EXTRA_DIST += shared/irq/irq_asm.h shared/irq/irq_asm.S
EXTRA_DIST += shared/irq/idt.c
EXTRA_DIST += shared/irq/irq_init.c

# shared/pci
EXTRA_DIST += shared/pci/pcibios.h
EXTRA_DIST += shared/pci/pcibios.c

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
