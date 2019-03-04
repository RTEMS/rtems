/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCLEON2
 *
 * @brief Global BSP definitions.
 */

/*  bsp.h
 *
 *  This include file contains all SPARC simulator definitions.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */

#ifndef LIBBSP_SPARC_LEON2_BSP_H
#define LIBBSP_SPARC_LEON2_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <leon.h>
#include <rtems/irq-extension.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSBSPsSPARCLEON2 LEON2
 *
 * @ingroup RTEMSBSPsSPARC
 *
 * @brief LEON2 Board Support Package.
 *
 * @{
 */

/* SPARC CPU variant: LEON2 */
#define LEON2 1

/*
 *  BSP provides its own Idle thread body
 */
void *bsp_idle_thread( uintptr_t ignored );
#define BSP_IDLE_TASK_BODY bsp_idle_thread

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int rtems_leon_open_eth_driver_attach(
  struct rtems_bsdnet_ifconfig *config
);
extern int rtems_smc91111_driver_attach_leon2(
  struct rtems_bsdnet_ifconfig *config
);
#define RTEMS_BSP_NETWORK_DRIVER_NAME "open_eth1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH_OPENETH \
          rtems_leon_open_eth_driver_attach
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH_SMC91111 \
          rtems_smc91111_driver_attach_leon2

#define HAS_SMC91111

/* Configure GRETH driver */
#define GRETH_SUPPORTED
#define GRETH_MEM_LOAD(addr) leon_r32_no_cache((uintptr_t) addr)

/*
 *  The synchronous trap is an arbitrarily chosen software trap.
 */

extern int   CPU_SPARC_HAS_SNOOPING;

/* Constants */

/*
 *  Information placed in the linkcmds file.
 */

extern int   RAM_START;
extern int   RAM_END;
extern int   RAM_SIZE;

extern int   PROM_START;
extern int   PROM_END;
extern int   PROM_SIZE;

extern int   CLOCK_SPEED;

extern int   end;        /* last address in the program */

/* miscellaneous stuff assumed to exist */

rtems_isr_entry set_vector(                     /* returns old vector */
    rtems_isr_entry     handler,                /* isr routine        */
    rtems_vector_number vector,                 /* vector number      */
    int                 type                    /* RTEMS or RAW intr  */
);

void BSP_fatal_exit(uint32_t error);

void bsp_spurious_initialize( void );

/* Interrupt Service Routine (ISR) pointer */
typedef void (*bsp_shared_isr)(void *arg);

/* Initializes the Shared System Interrupt service */
extern void BSP_shared_interrupt_init(void);

/* Called directly from IRQ trap handler TRAP[0x10..0x1F] = IRQ[0..15] */
void bsp_isr_handler(rtems_vector_number vector);

/* Registers a shared IRQ handler, and enable it at IRQ controller. Multiple
 * interrupt handlers may use the same IRQ number, all ISRs will be called
 * when an interrupt on that line is fired.
 *
 * Arguments
 *  irq       System IRQ number
 *  info      Optional Name of IRQ source
 *  isr       Function pointer to the ISR
 *  arg       Second argument to function isr
 */
static __inline__ int BSP_shared_interrupt_register
       (
       int irq,
       const char *info,
       bsp_shared_isr isr,
       void *arg
       )
{
       return rtems_interrupt_handler_install(irq, info,
                                       RTEMS_INTERRUPT_SHARED, isr, arg);
}

/* Unregister previously registered shared IRQ handler.
 *
 * Arguments
 *  irq       System IRQ number
 *  isr       Function pointer to the ISR
 *  arg       Second argument to function isr
 */
static __inline__ int BSP_shared_interrupt_unregister
       (
       int irq,
       bsp_shared_isr isr,
       void *arg
       )
{
       return rtems_interrupt_handler_remove(irq, isr, arg);
}

/* Clear interrupt pending on IRQ controller, this is typically done on a
 * level triggered interrupt source such as PCI to avoid taking double IRQs.
 * In such a case the interrupt source must be cleared first on LEON, before
 * acknowledging the IRQ with this function.
 *
 * Arguments
 *  irq       System IRQ number
 */
extern void BSP_shared_interrupt_clear(int irq);

/* Enable Interrupt. This function will unmask the IRQ at the interrupt
 * controller. This is normally done by _register(). Note that this will
 * affect all ISRs on this IRQ.
 *
 * Arguments
 *  irq       System IRQ number
 */
extern void BSP_shared_interrupt_unmask(int irq);

/* Disable Interrupt. This function will mask one IRQ at the interrupt
 * controller. This is normally done by _unregister().  Note that this will
 * affect all ISRs on this IRQ.
 *
 * Arguments
 *  irq         System IRQ number
 */
extern void BSP_shared_interrupt_mask(int irq);

/*
 * Delay method
 */
void rtems_bsp_delay(int usecs);

/*
 * Prototypes for BSP methods that are used across file boundaries
 */
int cchip1_register(void);

/* AT697 has PCI defined as big endian */
#define BSP_PCI_BIG_ENDIAN

/* Common driver build-time configurations. On small systems undefine
 * [DRIVER]_INFO_AVAIL to avoid info routines get dragged in. It is good
 * for debugging and printing information about the system, but makes the
 * image bigger.
 */
#define AMBAPPBUS_INFO_AVAIL          /* AMBAPP Bus driver */
#define GPTIMER_INFO_AVAIL            /* GPTIMER Timer driver */
#define GRETH_INFO_AVAIL              /* GRETH Ethernet driver */

void console_outbyte_polled( int port, unsigned char ch );

int console_inbyte_nonblocking( int port );

#ifdef __cplusplus
}
#endif

/** @} */

#endif
