/**
 * @file
 *
 * @ingroup raspberrypi_interrupt
 *
 * @brief Interrupt support.
 */

/*
 * Copyright (c) 2014 Andre Marques <andre.lousa.marques at gmail.com>
 *
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/armv4.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/raspberrypi.h>
#include <bsp/linker-symbols.h>
#include <bsp/mmu.h>
#include <rtems/bspIo.h>
#include <strings.h>

#ifdef RTEMS_SMP
#include <rtems/score/smp.h>
#include <rtems/score/smpimpl.h>
#endif

typedef struct {
  unsigned long enable_reg_addr;
  unsigned long disable_reg_addr;
} bcm2835_irq_ctrl_reg_t;

static const bcm2835_irq_ctrl_reg_t bcm2835_irq_ctrl_reg_table[] = {
  { BCM2835_IRQ_ENABLE1, BCM2835_IRQ_DISABLE1 },
  { BCM2835_IRQ_ENABLE2, BCM2835_IRQ_DISABLE2 },
  { BCM2835_IRQ_ENABLE_BASIC, BCM2835_IRQ_DISABLE_BASIC }
};

static inline const bcm2835_irq_ctrl_reg_t *
bsp_vector_to_reg(rtems_vector_number vector)
{
  return bcm2835_irq_ctrl_reg_table + (vector >> 5);
}

static inline uint32_t
bsp_vector_to_mask(rtems_vector_number vector)
{
  return 1 << (vector & 0x1f);
}

static const int bcm2835_irq_speedup_table[] =
{
  /*  0 */ BCM2835_IRQ_ID_BASIC_BASE_ID +  0,
  /*  1 */ BCM2835_IRQ_ID_BASIC_BASE_ID +  1,
  /*  2 */ BCM2835_IRQ_ID_BASIC_BASE_ID +  2,
  /*  3 */ BCM2835_IRQ_ID_BASIC_BASE_ID +  3,
  /*  4 */ BCM2835_IRQ_ID_BASIC_BASE_ID +  4,
  /*  5 */ BCM2835_IRQ_ID_BASIC_BASE_ID +  5,
  /*  6 */ BCM2835_IRQ_ID_BASIC_BASE_ID +  6,
  /*  7 */ BCM2835_IRQ_ID_BASIC_BASE_ID +  7,
  /*  8 */ -1, /* One or more bits set in pending register 1 */
  /*  9 */ -2, /* One or more bits set in pending register 2 */
  /* 10 */  7, /* GPU IRQ 7 */
  /* 11 */  9, /* GPU IRQ 9 */
  /* 12 */ 10, /* GPU IRQ 10 */
  /* 13 */ 18, /* GPU IRQ 18 */
  /* 14 */ 19, /* GPU IRQ 19 */
  /* 15 */ 53, /* GPU IRQ 53 */
  /* 16 */ 54, /* GPU IRQ 54 */
  /* 17 */ 55, /* GPU IRQ 55 */
  /* 18 */ 56, /* GPU IRQ 56 */
  /* 19 */ 57, /* GPU IRQ 57 */
  /* 20 */ 62, /* GPU IRQ 62 */
};

/*
 * Define which basic peding register (BCM2835_IRQ_BASIC) bits
 * should be processed through bcm2835_irq_speedup_table
 */

#define BCM2835_IRQ_BASIC_SPEEDUP_USED_BITS 0x1ffcff

/*
 * Determine the source of the interrupt and dispatch the correct handler.
 */
void bsp_interrupt_dispatch(void)
{
  unsigned int pend;
  unsigned int pend_bit;

  rtems_vector_number vector = 255;

#ifdef RTEMS_SMP
  uint32_t cpu_index_self = _SMP_Get_current_processor();
  uint32_t local_source = BCM2835_REG(BCM2836_IRQ_SOURCE_REG(cpu_index_self));

  if ( local_source & BCM2836_IRQ_SOURCE_MBOX3 ) {
    /* reset mailbox 3 contents to zero */
    BCM2835_REG(BCM2836_MAILBOX_3_READ_CLEAR_BASE + 0x10 * cpu_index_self) = 0xffffffff;
    _SMP_Inter_processor_interrupt_handler(
      _Per_CPU_Get_by_index(cpu_index_self)
    );
  }
  if ( cpu_index_self != 0 )
    return;
#endif /* RTEMS_SMP */

  pend = BCM2835_REG(BCM2835_IRQ_BASIC);
  if ( pend & BCM2835_IRQ_BASIC_SPEEDUP_USED_BITS ) {
    pend_bit = ffs(pend) - 1;
    vector = bcm2835_irq_speedup_table[pend_bit];
  } else {
    pend = BCM2835_REG(BCM2835_IRQ_PENDING1);
    if ( pend != 0 ) {
      pend_bit = ffs(pend) - 1;
      vector = pend_bit;
    } else {
      pend = BCM2835_REG(BCM2835_IRQ_PENDING2);
      if ( pend != 0 ) {
        pend_bit = ffs(pend) - 1;
        vector = pend_bit + 32;
      }
    }
  }

  if ( vector < 255 )
  {
      bsp_interrupt_handler_dispatch(vector);
  }
}

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BCM2835_REG(bsp_vector_to_reg(vector)->enable_reg_addr) =
              bsp_vector_to_mask(vector);
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  BCM2835_REG(bsp_vector_to_reg(vector)->disable_reg_addr) =
              bsp_vector_to_mask(vector);
}

void bsp_interrupt_handler_default(rtems_vector_number vector)
{
    printk("spurious interrupt: %lu\n", vector);
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
   BCM2835_REG(BCM2835_IRQ_DISABLE1) = 0xffffffff;
   BCM2835_REG(BCM2835_IRQ_DISABLE2) = 0xffffffff;
   BCM2835_REG(BCM2835_IRQ_DISABLE_BASIC) = 0xffffffff;
   BCM2835_REG(BCM2835_IRQ_FIQ_CTRL) = 0;
   return RTEMS_SUCCESSFUL;
}
