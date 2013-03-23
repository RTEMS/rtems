/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Interrupt support.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/score/armv4.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/raspberrypi.h>
#include <bsp/linker-symbols.h>
#include <bsp/mmu.h>

/*
** This sets the main exception vectors
*/
void raspberrypi_set_exception_handler(
                        Arm_symbolic_exception_name exception,
                        void (*handler)(void)
                                       )
{
    if ((unsigned) exception < MAX_EXCEPTIONS)
    {
        uint32_t *table = (uint32_t *) bsp_section_vector_begin + MAX_EXCEPTIONS;
        table [exception] = (uint32_t) handler;

    }
}

/*
** Determine the source of the interrupt and dispatch the correct handler.
*/
void bsp_interrupt_dispatch(void)
{
  rtems_vector_number vector = 255;

  /* ARM timer */
  if (BCM2835_REG(BCM2835_IRQ_BASIC) && 0x1)
  {
      vector = BCM2835_IRQ_ID_TIMER_0;

  }
  /* UART 0 */
  else if ( BCM2835_REG(BCM2835_IRQ_BASIC) && BCM2835_BIT(19))
  {
      vector = BCM2835_IRQ_ID_UART;
  }

  if ( vector < 255 )
  {
      bsp_interrupt_handler_dispatch(vector);
  }

}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  rtems_interrupt_level  level;

  rtems_interrupt_disable(level);

   /* ARM Timer */
  if ( vector == BCM2835_IRQ_ID_TIMER_0 )
  {
      BCM2835_REG(BCM2835_IRQ_ENABLE_BASIC) = 0x1;
  }
  /* UART 0 */
  else if ( vector == BCM2835_IRQ_ID_UART )
  {
      BCM2835_REG(BCM2835_IRQ_ENABLE2) =  BCM2835_BIT(25);

  }
  rtems_interrupt_enable(level);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  if ( vector == BCM2835_IRQ_ID_TIMER_0 )
  {
      BCM2835_REG(BCM2835_IRQ_DISABLE_BASIC) = 0x1;
  }
  else if ( vector == BCM2835_IRQ_ID_UART )
  {
      BCM2835_REG(BCM2835_IRQ_DISABLE2) = BCM2835_BIT(25);
  }
  rtems_interrupt_enable(level);

  return RTEMS_SUCCESSFUL;
}


void bsp_interrupt_handler_default(rtems_vector_number vector)
{
    printk("spurious interrupt: %u\n", vector);
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
   raspberrypi_set_exception_handler(ARM_EXCEPTION_IRQ, _ARMV4_Exception_interrupt);
   return RTEMS_SUCCESSFUL;
}
