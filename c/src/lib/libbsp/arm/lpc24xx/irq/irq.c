/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief LPC24XX interrupt support.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/lpc24xx.h>

void ExecuteITHandler( void)
{
  /* Read current vector number */
  rtems_vector_number vector = VICVectAddr;

  /* Acknowledge interrupt */
  VICVectAddr = 0;

  /* Dispatch interrupt handlers */
  bsp_interrupt_handler_dispatch( vector);
}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number vector)
{
  VICIntEnable = 1U << vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number vector)
{
  VICIntEnClear = 1U << vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize( void)
{
  volatile uint32_t *addr = VICVectAddrBase;
  volatile uint32_t *prio = VICVectPriorityBase;
  rtems_vector_number i = 0;

  /* Disable all interrupts */
  VICIntEnClear = 0xffffffff;

  /* Clear all software interrupts */
  VICSoftIntClear = 0xffffffff;

  /* Use IRQ category */
  VICIntSelect = 0;

  for (i = BSP_INTERRUPT_VECTOR_MIN; i <= BSP_INTERRUPT_VECTOR_MAX; ++i) {
    /* Use the vector address register to store the vector number */
    addr [i] = i;

    /* Give vector lowest priority */
    prio [i] = 15;
  }

  /* Reset priority mask register */
  VICSWPrioMask = 0xffff;

  /* Acknowledge interrupt */
  VICVectAddr = 0;

  /* Install the IRQ exception handler */
  _CPU_ISR_install_vector( ARM_EXCEPTION_IRQ, _ISR_Handler, NULL);

  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_handler_default( rtems_vector_number vector)
{
  printk( "Spurious interrupt: %u\n", vector);
}
