/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Interrupt support.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/beagle.h>
#include <bsp/linker-symbols.h>

/*
 * Mask out SIC 1 and 2 IRQ request. There is no need to mask out the FIQ,
 * since a pending FIQ would be a fatal error.  The default handler will be
 * invoked in this case.
 */
#define BEAGLE_MIC_STATUS_MASK (~0x3U)

typedef union {
  struct {
    uint32_t mic;
    uint32_t sic_1;
    uint32_t sic_2;
  } field;
  uint32_t fields_table [BEAGLE_IRQ_MODULE_COUNT];
} beagle_irq_fields;

static uint8_t beagle_irq_priority_table [BEAGLE_IRQ_COUNT];

static beagle_irq_fields beagle_irq_priority_masks [BEAGLE_IRQ_PRIORITY_COUNT];

static beagle_irq_fields beagle_irq_enable;

static inline bool beagle_irq_is_valid(rtems_vector_number vector)
{
    printk( "beagle_irq_is_valid" );
  return vector <= BSP_INTERRUPT_VECTOR_MAX;
}

static inline bool beagle_irq_priority_is_valid(unsigned priority)
{
    printk( "beagle_irq_priority_is_valid" );
 return priority <= BEAGLE_IRQ_PRIORITY_LOWEST;
}

#define BEAGLE_IRQ_BIT_OPS_DEFINE \
  unsigned bit = index & 0x1fU; \
  unsigned module = index >> 5

#define BEAGLE_IRQ_BIT_OPS_FOR_REG_DEFINE \
  BEAGLE_IRQ_BIT_OPS_DEFINE; \
  unsigned module_offset = module << 14; \
  volatile uint32_t *reg = (volatile uint32_t *) \
    ((volatile char *) &beagle.mic + module_offset + register_offset)

#define BEAGLE_IRQ_OFFSET_ER 0U
#define BEAGLE_IRQ_OFFSET_RSR 4U
#define BEAGLE_IRQ_OFFSET_SR 8U
#define BEAGLE_IRQ_OFFSET_APR 12U
#define BEAGLE_IRQ_OFFSET_ATR 16U
#define BEAGLE_IRQ_OFFSET_ITR 20U

static inline bool beagle_irq_is_bit_set_in_register(
  unsigned index,
  unsigned register_offset
)
{
    printk( "beagle_irq_is_bit_set_in_register" );
  BEAGLE_IRQ_BIT_OPS_FOR_REG_DEFINE;
  return *reg & (1U << bit);
}

static inline void beagle_irq_set_bit_in_register(
  unsigned index,
  unsigned register_offset
)
{
    printk( "beagle_irq_set_bit_in_register" );
  BEAGLE_IRQ_BIT_OPS_FOR_REG_DEFINE;

  *reg |= 1U << bit;
}

static inline void beagle_irq_clear_bit_in_register(
  unsigned index,
  unsigned register_offset
)
{
    printk( "beagle_irq_clear_bit_in_register" );
  BEAGLE_IRQ_BIT_OPS_FOR_REG_DEFINE;

  *reg &= ~(1U << bit);
}

static inline void beagle_irq_set_bit_in_field(
  unsigned index,
  beagle_irq_fields *fields
)
{
    printk( "beagle_irq_set_bit_in_field" );
  BEAGLE_IRQ_BIT_OPS_DEFINE;

  fields->fields_table [module] |= 1U << bit;
}

static inline void beagle_irq_clear_bit_in_field(
  unsigned index,
  beagle_irq_fields *fields
)
{
    printk( "beagle_irq_clear_bit_in_field" );
  BEAGLE_IRQ_BIT_OPS_DEFINE;

  fields->fields_table [module] &= ~(1U << bit);
}

static inline unsigned beagle_irq_get_index(uint32_t val)
{
    printk( "beagle_irq_get_index" );
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "clz %[val], %[val]\n"
    "rsb %[val], %[val], #31\n"
    ARM_SWITCH_BACK
    : [val] "=r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
    : "[val]" (val)
  );

  return val;
}

void beagle_irq_set_priority(rtems_vector_number vector, unsigned priority)
{
    printk( "beagle_irq_set_priority" );
  if (beagle_irq_is_valid(vector)) {
    rtems_interrupt_level level;
    unsigned i = 0;

    if (priority > BEAGLE_IRQ_PRIORITY_LOWEST) {
      priority = BEAGLE_IRQ_PRIORITY_LOWEST;
    }

    beagle_irq_priority_table [vector] = (uint8_t) priority;

    for (i = BEAGLE_IRQ_PRIORITY_HIGHEST; i <= priority; ++i) {
      rtems_interrupt_disable(level);
      beagle_irq_clear_bit_in_field(vector, &beagle_irq_priority_masks [i]);
      rtems_interrupt_enable(level);
    }

    for (i = priority + 1; i <= BEAGLE_IRQ_PRIORITY_LOWEST; ++i) {
      rtems_interrupt_disable(level);
      beagle_irq_set_bit_in_field(vector, &beagle_irq_priority_masks [i]);
      rtems_interrupt_enable(level);
    }
  }
}

unsigned beagle_irq_get_priority(rtems_vector_number vector)
{
    printk( "beagle_irq_get_priority" );
  if (beagle_irq_is_valid(vector)) {
    return beagle_irq_priority_table [vector];
  } else {
    return BEAGLE_IRQ_PRIORITY_LOWEST;
  }
}

void beagle_irq_set_activation_polarity(
  rtems_vector_number vector,
  beagle_irq_activation_polarity activation_polarity
)
{
    printk( "beagle_irq_set_activation_polarity" );
  if (beagle_irq_is_valid(vector)) {
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    if (activation_polarity == BEAGLE_IRQ_ACTIVE_HIGH_OR_RISING_EDGE) {
      beagle_irq_set_bit_in_register(vector, BEAGLE_IRQ_OFFSET_APR);
    } else {
      beagle_irq_clear_bit_in_register(vector, BEAGLE_IRQ_OFFSET_APR);
    }
    rtems_interrupt_enable(level);
  }
}

beagle_irq_activation_polarity beagle_irq_get_activation_polarity(
  rtems_vector_number vector
)
{
    printk( "beagle_irq_get_activation_polarity" );
  if (beagle_irq_is_valid(vector)) {
    if (beagle_irq_is_bit_set_in_register(vector, BEAGLE_IRQ_OFFSET_APR)) {
      return BEAGLE_IRQ_ACTIVE_HIGH_OR_RISING_EDGE;
    } else {
      return BEAGLE_IRQ_ACTIVE_LOW_OR_FALLING_EDGE;
    }
  } else {
    return BEAGLE_IRQ_ACTIVE_LOW_OR_FALLING_EDGE;
  }
}

void beagle_irq_set_activation_type(
  rtems_vector_number vector,
  beagle_irq_activation_type activation_type
)
{
    printk( "beagle_irq_set_activation_type" );
  if (beagle_irq_is_valid(vector)) {
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    if (activation_type == BEAGLE_IRQ_EDGE_SENSITIVE) {
      beagle_irq_set_bit_in_register(vector, BEAGLE_IRQ_OFFSET_ATR);
    } else {
      beagle_irq_clear_bit_in_register(vector, BEAGLE_IRQ_OFFSET_ATR);
    }
    rtems_interrupt_enable(level);
  }
}

beagle_irq_activation_type beagle_irq_get_activation_type(
  rtems_vector_number vector
)
{
    printk( "beagle_irq_get_activation_type" );
  if (beagle_irq_is_valid(vector)) {
    if (beagle_irq_is_bit_set_in_register(vector, BEAGLE_IRQ_OFFSET_ATR)) {
      return BEAGLE_IRQ_EDGE_SENSITIVE;
    } else {
      return BEAGLE_IRQ_LEVEL_SENSITIVE;
    }
  } else {
    return BEAGLE_IRQ_LEVEL_SENSITIVE;
  }
}

void bsp_interrupt_dispatch(void)
{
    printk( "bsp_interrupt_dispatch" );

  uint32_t status = beagle.mic.sr & BEAGLE_MIC_STATUS_MASK;
  uint32_t er_mic = beagle.mic.er;
  uint32_t er_sic_1 = beagle.sic_1.er;
  uint32_t er_sic_2 = beagle.sic_2.er;
  uint32_t psr = 0;
  beagle_irq_fields *masks = NULL;
  rtems_vector_number vector = 0;
  unsigned priority = 0;

  if (status != 0) {
    vector = beagle_irq_get_index(status);
  } else {
    status = beagle.sic_1.sr;
    if (status != 0) {
      vector = beagle_irq_get_index(status) + BEAGLE_IRQ_MODULE_SIC_1;
    } else {
      status = beagle.sic_2.sr;
      if (status != 0) {
        vector = beagle_irq_get_index(status) + BEAGLE_IRQ_MODULE_SIC_2;
      } else {
        return;
      }
    }
  }

  priority = beagle_irq_priority_table [vector];

  masks = &beagle_irq_priority_masks [priority];

  beagle.mic.er = er_mic & masks->field.mic;
  beagle.sic_1.er = er_sic_1 & masks->field.sic_1;
  beagle.sic_2.er = er_sic_2 & masks->field.sic_2;

  psr = arm_status_irq_enable();

  bsp_interrupt_handler_dispatch(vector);

  arm_status_restore(psr);

  beagle.mic.er = er_mic & beagle_irq_enable.field.mic;
  beagle.sic_1.er = er_sic_1 & beagle_irq_enable.field.sic_1;
  beagle.sic_2.er = er_sic_2 & beagle_irq_enable.field.sic_2;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
    printk( "bsp_interrupt_vector_enable" );
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  beagle_irq_set_bit_in_register(vector, BEAGLE_IRQ_OFFSET_ER);
  beagle_irq_set_bit_in_field(vector, &beagle_irq_enable);
  rtems_interrupt_enable(level);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
    printk( "bsp_interrupt_vector_disable" );
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  beagle_irq_clear_bit_in_field(vector, &beagle_irq_enable);
  beagle_irq_clear_bit_in_register(vector, BEAGLE_IRQ_OFFSET_ER);
  rtems_interrupt_enable(level);

  return RTEMS_SUCCESSFUL;
}

void beagle_set_exception_handler(
  Arm_symbolic_exception_name exception,
  void (*handler)(void)
)
{
    printk( "beagle_set_exception_handler" );
 if ((unsigned) exception < MAX_EXCEPTIONS) {
    uint32_t *table = (uint32_t *) bsp_vector_table_begin + MAX_EXCEPTIONS;

    table [exception] = (uint32_t) handler;

    #ifndef BEAGLE_DISABLE_MMU
      rtems_cache_flush_multiple_data_lines(table, 64);
      rtems_cache_invalidate_multiple_instruction_lines(NULL, 64);
    #endif
  }
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
    printk( "bsp_interrupt_facility_initialize" );
 size_t i = 0;

  /* Set default priority */
  for (i = 0; i < BEAGLE_IRQ_COUNT; ++i) {
    beagle_irq_priority_table [i] = BEAGLE_IRQ_PRIORITY_LOWEST;
  }

  /* Enable SIC 1 and 2 at all priorities */
  for (i = 0; i < BEAGLE_IRQ_PRIORITY_COUNT; ++i) {
    beagle_irq_priority_masks [i].field.mic = 0xc0000003;
  }

  /* Disable all interrupts except SIC 1 and 2 */
  beagle_irq_enable.field.sic_2 = 0x0;
  beagle_irq_enable.field.sic_1 = 0x0;
  beagle_irq_enable.field.mic = 0xc0000003;
  beagle.sic_1.er = 0x0;
  beagle.sic_2.er = 0x0;
  beagle.mic.er = 0xc0000003;

  /* Set interrupt types to IRQ */
  beagle.mic.itr = 0x0;
  beagle.sic_1.itr = 0x0;
  beagle.sic_2.itr = 0x0;

  /* Set interrupt activation polarities */
  beagle.mic.apr = 0x3ff0efe0;
  beagle.sic_1.apr = 0xfbd27184;
  beagle.sic_2.apr = 0x801810c0;

  /* Set interrupt activation types */
  beagle.mic.atr = 0x0;
  beagle.sic_1.atr = 0x26000;
  beagle.sic_2.atr = 0x0;

  beagle_set_exception_handler(ARM_EXCEPTION_IRQ, arm_exc_interrupt);

  return RTEMS_SUCCESSFUL;
}
