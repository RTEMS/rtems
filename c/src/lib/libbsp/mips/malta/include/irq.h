/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Malta Interrupt Definitions
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_MIPS_MALTA_IRQ_H
#define LIBBSP_MIPS_MALTA_IRQ_H

#ifndef ASM
  #include <rtems.h>
  #include <rtems/irq.h>
  #include <rtems/irq-extension.h>
  #include <rtems/score/mips.h>
#endif

/**
 * @addtogroup bsp_interrupt
 *
 * @{
 */

#define BSP_INTERRUPT_VECTOR_MIN 0

/*
 *  Interrupt Vector Numbers
 *
 *  NOTE: Numbers 0-15 directly map to levels on the IRC.
 *        Number 16 is "1xxxx" per p. 164 of the TX3904 manual.
 */
#define MALTA_CPU_INT_START           MIPS_INTERRUPT_BASE+0
#define MALTA_CPU_INT_SW0             MALTA_CPU_INT_START+0
#define MALTA_CPU_INT_SW2             MALTA_CPU_INT_START+1
#define MALTA_CPU_INT0                MALTA_CPU_INT_START+2
#define MALTA_CPU_INT1                MALTA_CPU_INT_START+3
#define MALTA_CPU_INT2                MALTA_CPU_INT_START+4
#define MALTA_CPU_INT3                MALTA_CPU_INT_START+5
#define MALTA_CPU_INT4                MALTA_CPU_INT_START+6
#define MALTA_CPU_INT5                MALTA_CPU_INT_START+7
#define MALTA_CPU_INT_LAST            MALTA_CPU_INT5

#define MALTA_SB_IRQ_START            MALTA_CPU_INT_LAST+1
#define MALTA_SB_IRQ_0                MALTA_SB_IRQ_START+0
#define MALTA_SB_IRQ_1                MALTA_SB_IRQ_START+1
#define MALTA_SB_IRQ_2                MALTA_SB_IRQ_START+2
#define MALTA_SB_IRQ_3                MALTA_SB_IRQ_START+3
#define MALTA_SB_IRQ_4                MALTA_SB_IRQ_START+4
#define MALTA_SB_IRQ_5                MALTA_SB_IRQ_START+5
#define MALTA_SB_IRQ_6                MALTA_SB_IRQ_START+6
#define MALTA_SB_IRQ_7                MALTA_SB_IRQ_START+7
#define MALTA_SB_IRQ_8                MALTA_SB_IRQ_START+8
#define MALTA_SB_IRQ_9                MALTA_SB_IRQ_START+9
#define MALTA_SB_IRQ_10               MALTA_SB_IRQ_START+10
#define MALTA_SB_IRQ_11               MALTA_SB_IRQ_START+11
#define MALTA_SB_IRQ_12               MALTA_SB_IRQ_START+12
#define MALTA_SB_IRQ_13               MALTA_SB_IRQ_START+13
#define MALTA_SB_IRQ_14               MALTA_SB_IRQ_START+14
#define MALTA_SB_IRQ_15               MALTA_SB_IRQ_START+15
#define MALTA_SB_IRQ_LAST             MALTA_SB_IRQ_15

#define MALTA_PCI_ADP_START           MALTA_SB_IRQ_LAST+1
#define MALTA_PCI_ADP20               MALTA_PCI_ADP_START+0
#define MALTA_PCI_ADP21               MALTA_PCI_ADP_START+1
#define MALTA_PCI_ADP22               MALTA_PCI_ADP_START+2
#define MALTA_PCI_ADP27               MALTA_PCI_ADP_START+3
#define MALTA_PCI_ADP28               MALTA_PCI_ADP_START+4
#define MALTA_PCI_ADP29               MALTA_PCI_ADP_START+5
#define MALTA_PCI_ADP30               MALTA_PCI_ADP_START+6
#define MALTA_PCI_ADP31               MALTA_PCI_ADP_START+7
#define MALTA_PCI_ADP_LAST            MALTA_PCI_ADP31
#

#define BSP_INTERRUPT_VECTOR_MAX   MALTA_PCI_ADP_LAST

/*
 * Redefine interrupts with more descriptive names.
 * The Generic ones above match the hardware name,
 * where these match the device name.
 */
#define MALTA_INT_SOUTHBRIDGE_INTR             MALTA_CPU_INT0
#define MALTA_INT_SOUTHBRIDGE_SMI              MALTA_CPU_INT1
#define MALTA_INT_TTY2                         MALTA_CPU_INT2
#define MALTA_INT_COREHI                       MALTA_CPU_INT3
#define MALTA_INT_CORELO                       MALTA_CPU_INT4
#define MALTA_INT_TICKER                       MALTA_CPU_INT5

#define MALTA_IRQ_TIMER_SOUTH_BRIDGE           MALTA_SB_IRQ_0
#define MALTA_IRQ_KEYBOARD_SUPERIO             MALTA_SB_IRQ_1
#define MALTA_IRQ_RESERVED1_SOUTH_BRIDGE       MALTA_SB_IRQ_2
#define MALTA_IRQ_TTY1                         MALTA_SB_IRQ_3
#define MALTA_IRQ_TTY0                         MALTA_SB_IRQ_4
#define MALTA_IRQ_NOT_USED                     MALTA_SB_IRQ_5
#define MALTA_IRQ_FLOPPY_SUPERIO               MALTA_SB_IRQ_6
#define MALTA_IRQ_PARALLEL_PORT_SUPERIO        MALTA_SB_IRQ_7
#define MALTA_IRQ_REALTIME_CLOCK_SOUTH_BRIDGE  MALTA_SB_IRQ_8
#define MALTA_IRQ_I2C_SOUTH_BRIDGE             MALTA_SB_IRQ_9
/* PCI A, PCI B (including Ethernet) PCI slot 1..4, Ethernet */
#define MALTA_IRQ_PCI_A_B                      MALTA_SB_IRQ_10
/* PCI slot 1..4 (audio, USB)  */
#define MALTA_IRQ_PCI_C_D                      MALTA_SB_IRQ_11
#define MALTA_IRQ_MOUSE_SUPERIO                MALTA_SB_IRQ_12
#define MALTA_IRQ_RESERVED2_SOUTH_BRIDGE       MALTA_SB_IRQ_13
#define MALTA_IRQ_PRIMARY_IDE                  MALTA_SB_IRQ_14
#define MALTA_IRQ_SECONDARY_IDE                MALTA_SB_IRQ_15
#define MALTA_IRQ_SOUTH_BRIDGE    MALTA_PCI_ADP20
#define MALTA_IRQ_ETHERNET        MALTA_IRQ_PCI_A_B
#define MALTA_IRQ_AUDIO           MALTA_PCI_ADP22
#define MALTA_IRQ_CORE_CARD       MALTA_PCI_ADP27
#define MALTA_IRQ_PCI_CONNECTOR_1 MALTA_PCI_ADP28
#define MALTA_IRQ_PCI_CONNECTOR_2 MALTA_PCI_ADP29
#define MALTA_IRQ_PCI_CONNECTOR_3 MALTA_PCI_ADP30
#define MALTA_IRQ_PCI_CONNECTOR_4 MALTA_PCI_ADP31

#ifndef ASM

#endif /* ASM */

/** @} */

#endif /* LIBBSP_MIPS_MALTA_IRQ_H */
