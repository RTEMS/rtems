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

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/lpc32xx.h>
#include <bsp/linker-symbols.h>
#include <bsp/mmu.h>

/*
 * Mask out SIC 1 and 2 IRQ request. There is no need to mask out the FIQ,
 * since a pending FIQ would be a fatal error.  The default handler will be
 * invoked in this case.
 */
#define LPC32XX_MIC_STATUS_MASK (~0x3U)

typedef union {
  struct {
    uint32_t mic;
    uint32_t sic_1;
    uint32_t sic_2;
  } field;
  uint32_t fields_table [LPC32XX_IRQ_MODULE_COUNT];
} lpc32xx_irq_fields;

static uint8_t lpc32xx_irq_priority_table [LPC32XX_IRQ_COUNT];

static lpc32xx_irq_fields lpc32xx_irq_priority_masks [LPC32XX_IRQ_PRIORITY_COUNT];

static lpc32xx_irq_fields lpc32xx_irq_enable;

static inline bool lpc32xx_irq_is_valid(rtems_vector_number vector)
{
  return vector <= BSP_INTERRUPT_VECTOR_MAX;
}

static inline bool lpc32xx_irq_priority_is_valid(unsigned priority)
{
  return priority <= LPC32XX_IRQ_PRIORITY_LOWEST;
}

#define LPC32XX_IRQ_BIT_OPS_DEFINE \
  unsigned bit = index & 0x1fU; \
  unsigned module = index >> 5

#define LPC32XX_IRQ_BIT_OPS_FOR_REG_DEFINE \
  LPC32XX_IRQ_BIT_OPS_DEFINE; \
  unsigned module_offset = module << 14; \
  volatile uint32_t *reg = (volatile uint32_t *) \
    ((volatile char *) &lpc32xx.mic + module_offset + register_offset)

#define LPC32XX_IRQ_OFFSET_ER 0U
#define LPC32XX_IRQ_OFFSET_RSR 4U
#define LPC32XX_IRQ_OFFSET_SR 8U
#define LPC32XX_IRQ_OFFSET_APR 12U
#define LPC32XX_IRQ_OFFSET_ATR 16U
#define LPC32XX_IRQ_OFFSET_ITR 20U

static inline bool lpc32xx_irq_is_bit_set_in_register(unsigned index, unsigned register_offset)
{
  LPC32XX_IRQ_BIT_OPS_FOR_REG_DEFINE;

  return *reg & (1U << bit);
}

static inline void lpc32xx_irq_set_bit_in_register(unsigned index, unsigned register_offset)
{
  LPC32XX_IRQ_BIT_OPS_FOR_REG_DEFINE;

  *reg |= 1U << bit;
}

static inline void lpc32xx_irq_clear_bit_in_register(unsigned index, unsigned register_offset)
{
  LPC32XX_IRQ_BIT_OPS_FOR_REG_DEFINE;

  *reg &= ~(1U << bit);
}

static inline void lpc32xx_irq_set_bit_in_field(unsigned index, lpc32xx_irq_fields *fields)
{
  LPC32XX_IRQ_BIT_OPS_DEFINE;

  fields->fields_table [module] |= 1U << bit;
}

static inline void lpc32xx_irq_clear_bit_in_field(unsigned index, lpc32xx_irq_fields *fields)
{
  LPC32XX_IRQ_BIT_OPS_DEFINE;

  fields->fields_table [module] &= ~(1U << bit);
}

static inline unsigned lpc32xx_irq_get_index(uint32_t val)
{
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

void lpc32xx_irq_set_priority(rtems_vector_number vector, unsigned priority)
{
  if (lpc32xx_irq_is_valid(vector)) {
    rtems_interrupt_level level;
    unsigned i = 0;

    if (priority > LPC32XX_IRQ_PRIORITY_LOWEST) {
      priority = LPC32XX_IRQ_PRIORITY_LOWEST;
    }

    lpc32xx_irq_priority_table [vector] = (uint8_t) priority;

    for (i = LPC32XX_IRQ_PRIORITY_HIGHEST; i <= priority; ++i) {
      rtems_interrupt_disable(level);
      lpc32xx_irq_clear_bit_in_field(vector, &lpc32xx_irq_priority_masks [i]);
      rtems_interrupt_enable(level);
    }

    for (i = priority + 1; i <= LPC32XX_IRQ_PRIORITY_LOWEST; ++i) {
      rtems_interrupt_disable(level);
      lpc32xx_irq_set_bit_in_field(vector, &lpc32xx_irq_priority_masks [i]);
      rtems_interrupt_enable(level);
    }
  }
}

unsigned lpc32xx_irq_get_priority(rtems_vector_number vector)
{
  if (lpc32xx_irq_is_valid(vector)) {
    return lpc32xx_irq_priority_table [vector];
  } else {
    return LPC32XX_IRQ_PRIORITY_LOWEST;
  }
}

void lpc32xx_irq_set_activation_polarity(rtems_vector_number vector, lpc32xx_irq_activation_polarity activation_polarity)
{
  if (lpc32xx_irq_is_valid(vector)) {
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    if (activation_polarity == LPC32XX_IRQ_ACTIVE_HIGH_OR_RISING_EDGE) {
      lpc32xx_irq_set_bit_in_register(vector, LPC32XX_IRQ_OFFSET_APR);
    } else {
      lpc32xx_irq_clear_bit_in_register(vector, LPC32XX_IRQ_OFFSET_APR);
    }
    rtems_interrupt_enable(level);
  }
}

lpc32xx_irq_activation_polarity lpc32xx_irq_get_activation_polarity(rtems_vector_number vector)
{
  if (lpc32xx_irq_is_valid(vector)) {
    if (lpc32xx_irq_is_bit_set_in_register(vector, LPC32XX_IRQ_OFFSET_APR)) {
      return LPC32XX_IRQ_ACTIVE_HIGH_OR_RISING_EDGE;
    } else {
      return LPC32XX_IRQ_ACTIVE_LOW_OR_FALLING_EDGE;
    }
  } else {
    return LPC32XX_IRQ_ACTIVE_LOW_OR_FALLING_EDGE;
  }
}

void lpc32xx_irq_set_activation_type(rtems_vector_number vector, lpc32xx_irq_activation_type activation_type)
{
  if (lpc32xx_irq_is_valid(vector)) {
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    if (activation_type == LPC32XX_IRQ_EDGE_SENSITIVE) {
      lpc32xx_irq_set_bit_in_register(vector, LPC32XX_IRQ_OFFSET_ATR);
    } else {
      lpc32xx_irq_clear_bit_in_register(vector, LPC32XX_IRQ_OFFSET_ATR);
    }
    rtems_interrupt_enable(level);
  }
}

lpc32xx_irq_activation_type lpc32xx_irq_get_activation_type(rtems_vector_number vector)
{
  if (lpc32xx_irq_is_valid(vector)) {
    if (lpc32xx_irq_is_bit_set_in_register(vector, LPC32XX_IRQ_OFFSET_ATR)) {
      return LPC32XX_IRQ_EDGE_SENSITIVE;
    } else {
      return LPC32XX_IRQ_LEVEL_SENSITIVE;
    }
  } else {
    return LPC32XX_IRQ_LEVEL_SENSITIVE;
  }
}

void bsp_interrupt_dispatch(void)
{
  uint32_t status = lpc32xx.mic.sr & LPC32XX_MIC_STATUS_MASK;
  uint32_t er_mic = lpc32xx.mic.er;
  uint32_t er_sic_1 = lpc32xx.sic_1.er;
  uint32_t er_sic_2 = lpc32xx.sic_2.er;
  uint32_t psr = 0;
  lpc32xx_irq_fields *masks = NULL;
  rtems_vector_number vector = 0;
  unsigned priority = 0;

  if (status != 0) {
    vector = lpc32xx_irq_get_index(status);
  } else {
    status = lpc32xx.sic_1.sr;
    if (status != 0) {
      vector = lpc32xx_irq_get_index(status) + LPC32XX_IRQ_MODULE_SIC_1;
    } else {
      status = lpc32xx.sic_2.sr;
      if (status != 0) {
        vector = lpc32xx_irq_get_index(status) + LPC32XX_IRQ_MODULE_SIC_2;
      } else {
        return;
      }
    }
  }

  priority = lpc32xx_irq_priority_table [vector];

  masks = &lpc32xx_irq_priority_masks [priority];

  lpc32xx.mic.er = er_mic & masks->field.mic;
  lpc32xx.sic_1.er = er_sic_1 & masks->field.sic_1;
  lpc32xx.sic_2.er = er_sic_2 & masks->field.sic_2;

  psr = arm_status_irq_enable();

  bsp_interrupt_handler_dispatch(vector);

  arm_status_restore(psr);

  lpc32xx.mic.er = er_mic & lpc32xx_irq_enable.field.mic;
  lpc32xx.sic_1.er = er_sic_1 & lpc32xx_irq_enable.field.sic_1;
  lpc32xx.sic_2.er = er_sic_2 & lpc32xx_irq_enable.field.sic_2;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  lpc32xx_irq_set_bit_in_register(vector, LPC32XX_IRQ_OFFSET_ER);
  lpc32xx_irq_set_bit_in_field(vector, &lpc32xx_irq_enable);
  rtems_interrupt_enable(level);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  lpc32xx_irq_clear_bit_in_field(vector, &lpc32xx_irq_enable);
  lpc32xx_irq_clear_bit_in_register(vector, LPC32XX_IRQ_OFFSET_ER);
  rtems_interrupt_enable(level);

  return RTEMS_SUCCESSFUL;
}

void lpc32xx_set_exception_handler(
  Arm_symbolic_exception_name exception,
  void (*handler)(void)
)
{
  if ((unsigned) exception < MAX_EXCEPTIONS) {
    uint32_t *table = (uint32_t *) bsp_vector_table_begin + MAX_EXCEPTIONS;

    table [exception] = (uint32_t) handler;

    #ifndef LPC32XX_DISABLE_MMU
      rtems_cache_flush_multiple_data_lines(table, 64);
      rtems_cache_invalidate_multiple_instruction_lines(NULL, 64);
    #endif
  }
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  size_t i = 0;

  /* Set default priority */
  for (i = 0; i < LPC32XX_IRQ_COUNT; ++i) {
    lpc32xx_irq_priority_table [i] = LPC32XX_IRQ_PRIORITY_LOWEST;
  }

  /* Enable SIC 1 and 2 at all priorities */
  for (i = 0; i < LPC32XX_IRQ_PRIORITY_COUNT; ++i) {
    lpc32xx_irq_priority_masks [i].field.mic = 0xc0000003;
  }

  /* Disable all interrupts except SIC 1 and 2 */
  lpc32xx_irq_enable.field.sic_2 = 0x0;
  lpc32xx_irq_enable.field.sic_1 = 0x0;
  lpc32xx_irq_enable.field.mic = 0xc0000003;
  lpc32xx.sic_1.er = 0x0;
  lpc32xx.sic_2.er = 0x0;
  lpc32xx.mic.er = 0xc0000003;

  /* Set interrupt types to IRQ */
  lpc32xx.mic.itr = 0x0;
  lpc32xx.sic_1.itr = 0x0;
  lpc32xx.sic_2.itr = 0x0;

  /* Set interrupt activation polarities */
  lpc32xx.mic.apr = 0x3ff0efe0;
  lpc32xx.sic_1.apr = 0xfbd27184;
  lpc32xx.sic_2.apr = 0x801810c0;

  /* Set interrupt activation types */
  lpc32xx.mic.atr = 0x0;
  lpc32xx.sic_1.atr = 0x26000;
  lpc32xx.sic_2.atr = 0x0;

  lpc32xx_set_exception_handler(ARM_EXCEPTION_IRQ, arm_exc_interrupt);

  return RTEMS_SUCCESSFUL;
}
