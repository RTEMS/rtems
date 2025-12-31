/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2025 Jeremy Lorelli <lorelli@slac.stanford.edu>
 * Copyright (c) 2005 Eric Norum <eric@norum.ca>
 * COPYRIGHT (c) 2005.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */
#include <bsp.h>
#include <bsp/irq.h>
#include <mcf5282/mcf5282.h>

#include <bsp/irq-generic.h>

typedef void(*void_func_t)(void);

extern void bsp_default_isr_handler(int);

/*
 * Adjust interrupt vector to an offset in INTCn register
 * MCF5282 interrupt vector mapping:
 *  * 0-63    -- internal CF interrupts
 *  * 64-127  -- INTC0
 *  * 128-191 -- INTC1
 *  * 192-255 -- Unused by hardware; used for fake interrupts in software
 */
static int vector_to_offset(int vector)
{
  if (vector < 128 && vector >= 64)
    return vector - 64;
  else if (vector < 192)
    return vector - 128;
  return 0;
}

/*
 * Get the associated INTCn chip.
 * 0 == INTC0, 1 == INTC1, -1 == neither
 */
static int vector_to_source(int vector)
{
  if (vector < 128 && vector >= 64)
    return 0;
  else if (vector < 192 && vector >= 128)
    return 1;
  return -1;
}

bool bsp_interrupt_is_valid_vector(rtems_vector_number vector)
{
  return vector < BSP_INTERRUPT_VECTOR_COUNT;
}

static uint32_t s_wasInstalled[6];

static bool bsp_interrupt_was_installed(int vector)
{
  return !!(s_wasInstalled[vector/32] & (1<<vector%32));
}

static void bsp_interrupt_mark_installed(int vector)
{
  s_wasInstalled[vector/32] |= (1<<vector%32);
}

/*
 * Interrupt controller allocation. Mark a level and priority used,
 * each interrupt must have a unique level/prio
 */
rtems_status_code bsp_allocate_interrupt(int level, int priority)
{
  static char used[7];
  rtems_interrupt_level l;
  rtems_status_code ret = RTEMS_RESOURCE_IN_USE;

  if ((level < 1) || (level > 7) || (priority < 0) || (priority > 7))
    return RTEMS_INVALID_NUMBER;
  rtems_interrupt_disable(l);
  if ((used[level-1] & (1 << priority)) == 0) {
    used[level-1] |= (1 << priority);
    ret = RTEMS_SUCCESSFUL;
  }
  rtems_interrupt_enable(l);
  return ret;
}

static void enable_irq(int which, int off)
{
  if (off < 32) {
    *(which ? &MCF5282_INTC1_IMRL : &MCF5282_INTC0_IMRL) &=
      ~(1<<off | MCF5282_INTC_IMRL_MASKALL);
  }
  else {
    *(which ? &MCF5282_INTC1_IMRH : &MCF5282_INTC0_IMRH) &= ~(1<<(off-32));
  }
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  int off;
  int which;
  int level;
  int prio;
  volatile uint8_t* reg;

  if (!bsp_interrupt_is_valid_vector(vector))
    return RTEMS_UNSATISFIED;

  if ((which = vector_to_source(vector)) < 0) {
    return RTEMS_SUCCESSFUL; /* nothing to do */
  }

  off = vector_to_offset(vector);

  /* check if the interrupt has already been setup */
  if (!bsp_interrupt_was_installed(vector)) {
    bsp_interrupt_mark_installed(vector);

    /* attempt to map a unique priority/level to the interrupt. Uniqueness is
     * required, otherwise we get UB */
    for (level = 1; level < 8; level++) {
      for (prio = 0; prio < 8; prio++) {
        if (bsp_allocate_interrupt(level, prio) == RTEMS_SUCCESSFUL) {
          MCF5282_EPORT_EPIER;
          /* ICR1-8 are fixed level/prio. the rest need setup */
          if (off > 8) {
            reg = (which ? &MCF5282_INTC1_ICR09 - 8 : &MCF5282_INTC0_ICR1);
            reg += off - 1;

            /* configure level and prio */
            *reg |= MCF5282_INTC_ICR_IL(level) |
              MCF5282_INTC_ICR_IP(prio);
          }

          enable_irq(which, off);
          return RTEMS_SUCCESSFUL;
        }
      }
    }

    return RTEMS_UNSATISFIED;
  }

  enable_irq(which, off);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  (void) vector;
  (void) attributes;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  int off;
  int which;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if ((which = vector_to_source(vector)) < 0) {
    return RTEMS_UNSATISFIED;
  }

  off = vector_to_offset(vector);

  if (off < 32)
    *(which ? &MCF5282_INTC1_INTFRCL : &MCF5282_INTC0_INTFRCL) |= (1<<off);
  else
    *(which ? &MCF5282_INTC1_INTFRCH : &MCF5282_INTC0_INTFRCH) |= (1<<(off-32));

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  int off;
  int which;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if ((which = vector_to_source(vector)) < 0) {
    return RTEMS_SUCCESSFUL; /* nothing to do */
  }

  off = vector_to_offset(vector);

  /* mask the interrupt */
  if (off < 32) {
    *(which ? &MCF5282_INTC1_IMRL : &MCF5282_INTC0_IMRL) |= 1<<off;
  }
  else {
    *(which ? &MCF5282_INTC1_IMRH : &MCF5282_INTC0_IMRH) |= 1<<(off-32);
  }
  
  
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_set_priority(
  rtems_vector_number vector,
  uint32_t priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_get_priority(
  rtems_vector_number vector,
  uint32_t *priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(priority != NULL);
  return RTEMS_UNSATISFIED;
}

void bsp_interrupt_facility_initialize(void)
{
}

static void set_exception_handler(
  rtems_vector_number vector,
  void_func_t          handler
)
{
  void **vbr;
  void_func_t *exception_table;

  m68k_get_vbr(vbr);

  exception_table = (void_func_t*)vbr;
  exception_table[vector] = handler;
}

static void dispatch_handler(rtems_vector_number exception_vector)
{
  bsp_interrupt_handler_dispatch_unchecked(exception_vector);
}

void uC5282_interrupt_vector_install(rtems_vector_number vector)
{
  _ISR_Vector_table[vector] = dispatch_handler;
  set_exception_handler(vector, _ISR_Handler);
}

void uC5282_interrupt_vector_remove(rtems_vector_number vector)
{
  set_exception_handler(vector, (void_func_t)bsp_default_isr_handler);
}

/*
 * 'VME' interrupt support
 * Interrupt vectors 192-255 are set aside for use by external logic which
 * drives IRQ1*.  The actual interrupt source is read from the external
 * logic at FPGA_IRQ_INFO.  The most-significant bit of the least-significant
 * byte read from this location is set as long as the external logic has
 * interrupts to be serviced.  The least-significant six bits indicate the
 * interrupt source within the external logic and are used to select the
 * specified interrupt handler.
 */
#define FPGA_VECTOR (64+1)  /* IRQ1* pin connected to external FPGA */
#define FPGA_IRQ_INFO    *((vuint16 *)(0x31000000 + 0xfffffe))

#define NVECTORS (BSP_INTERRUPT_VECTOR_COUNT + BSP_VME_INTERRUPT_VECTOR_COUNT)

static struct handlerTab {
  BSP_VME_ISR_t func;
  void         *arg;
} handlerTab[NVECTORS];

BSP_VME_ISR_t BSP_getVME_isr(unsigned long vector, void **pusrArg)
{
  if (vector >= NVECTORS)
    return (BSP_VME_ISR_t)NULL;
  if (pusrArg)
    *pusrArg = handlerTab[vector].arg;
  return handlerTab[vector].func;
}

/*
 * Trampoline for FPGA interrupts on the IRQ1* pin
 */
static void fpga_trampoline(void* arg)
{
  rtems_vector_number v = (rtems_vector_number)arg;

  /* ACK the interrupt */
  MCF5282_EPORT_EPFR |= MCF5282_EPORT_EPFR_EPF1;

  /*
   * Handle FPGA interrupts until all have been consumed
   */
  int loopcount = 0;
  while (((v = FPGA_IRQ_INFO) & 0x80) != 0) {
    v = 192 + (v & 0x3f);
    if (++loopcount >= 50) {
      rtems_interrupt_level level;
      rtems_interrupt_disable(level);
      printk(
        "\nTOO MANY FPGA INTERRUPTS (LAST WAS 0x%x) -- "
        "DISABLING ALL FPGA INTERRUPTS.\n",
        v & 0x3f
      );
      MCF5282_INTC0_IMRL |= MCF5282_INTC_IMRL_INT1;
      rtems_interrupt_enable(level);
      return;
    }

    if (handlerTab[v].func)  {
      (*handlerTab[v].func)(handlerTab[v].arg, (unsigned long)v);
    }
    else {
      rtems_interrupt_level level;
      rtems_vector_number nv;
      rtems_interrupt_disable(level);
      printk("\nSPURIOUS FPGA INTERRUPT (0x%x).\n", v & 0x3f);
      if ((((nv = FPGA_IRQ_INFO) & 0x80) != 0)
          && ((nv & 0x3f) == (v & 0x3f))) {
        printk("DISABLING ALL FPGA INTERRUPTS.\n");
        MCF5282_INTC0_IMRL |= MCF5282_INTC_IMRL_INT1;
      }
      rtems_interrupt_enable(level);
      return;
    }
  }
}

/*
 * Trampoline for legacy interrupts installed via BSP_installVME_isr
 */
static void trampoline(void* arg)
{
  rtems_vector_number vec = (rtems_vector_number)arg;
  if (handlerTab[vec].func)
    (*handlerTab[vec].func)(handlerTab[vec].arg, (unsigned long)vec);
}

int BSP_installVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *usrArg)
{
  rtems_interrupt_level level;
  int i;

  /*
   * Register the handler information
   */
  if (vector >= NVECTORS)
    return -1;

  handlerTab[vector].func = handler;
  handlerTab[vector].arg = usrArg;

  rtems_interrupt_disable(level);

  /*
   * If this is an external FPGA ('VME') vector set up the real IRQ.
   */
  if ((vector >= BSP_FIRST_VME_INTERRUPT_VECTOR) && (vector <= 255)) {
    static volatile int setupDone;
    if (setupDone) {
      rtems_interrupt_enable(level);
      return 0;
    }
    setupDone = 1;

    i = rtems_interrupt_handler_install(
      FPGA_VECTOR,
      "fpga_trampoline",
      RTEMS_INTERRUPT_UNIQUE,
      fpga_trampoline,
      (void*)FPGA_VECTOR
    );

    if (i == RTEMS_SUCCESSFUL)
      i = bsp_interrupt_vector_enable(FPGA_VECTOR);

    /* enable IRQ1 EPORT interrupts as well */
    MCF5282_EPORT_EPIER |= MCF5282_EPORT_EPIER_EPIE1;

    rtems_interrupt_enable(level);
    return i;
  }

  /*
   * Make the connection between the interrupt and the local handler
   */
  i = rtems_interrupt_handler_install(
    vector,
    "trampoline",
    RTEMS_INTERRUPT_UNIQUE,
    trampoline,
    (void*)vector
  );

  if (i != RTEMS_SUCCESSFUL) {
    rtems_interrupt_enable(level);
    return i;
  }

  /* for compatibility with older code: Enable IRQn interrupts on EPORT too.
   * This used to be done in init_intc0_bit */
  if (vector <= (64 + 7) && vector > 64) {
    MCF5282_EPORT_EPIER |= 1 << (vector - 64);
  }

  i = bsp_interrupt_vector_enable(vector);

  rtems_interrupt_enable(level);
  return i;
}

int BSP_removeVME_isr(unsigned long vector, BSP_VME_ISR_t handler, void *usrArg)
{
  if (vector >= NVECTORS)
    return -1;
  if ((handlerTab[vector].func != handler)
     || (handlerTab[vector].arg != usrArg))
    return -1;
  handlerTab[vector].func = (BSP_VME_ISR_t)NULL;
  return 0;
}
