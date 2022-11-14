/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVGRLIBAMBA
 */

/*
 *  AMBA Plag & Play Bus Driver Macros
 *
 *  Macros used for AMBA Plug & Play bus scanning
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AMBA_H__
#define __AMBA_H__

/**
 * @defgroup RTEMSBSPsRISCVGRLIBAMBA RISC-V AMBA Driver Handler
 *
 * @ingroup RTEMSBSPsRISCVGRLIB
 *
 * @ingroup RTEMSBSPsSharedGRLIB
 *
 * @brief AMBA Plag & Play Bus Driver Macros
 *
 * @{
 */

#ifndef GRLIB_IO_AREA
#define GRLIB_IO_AREA 0xfff00000
#endif

#define GRLIB_CONF_AREA 0xff000
#define GRLIB_AHB_SLAVE_CONF_AREA (1 << 11)

#define GRLIB_AHB_CONF_WORDS 8
#define GRLIB_APB_CONF_WORDS 2
#define GRLIB_AHB_MASTERS 64
#define GRLIB_AHB_SLAVES 64
#define GRLIB_APB_SLAVES 16

#if defined(RTEMS_MULTIPROCESSING)
  #define GRLIB_CLOCK_INDEX \
   (rtems_configuration_get_user_multiprocessing_table() ? GRLIB_Cpu_Index : 0)
#else
  #define GRLIB_CLOCK_INDEX 0
#endif

#if defined(RTEMS_SMP)
#define GRLIB_COUNTER_GPTIMER_INDEX (GRLIB_CLOCK_INDEX + 1)
#else
#define GRLIB_COUNTER_GPTIMER_INDEX GRLIB_CLOCK_INDEX
#endif

#define GRLIB_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER 1000000

#define GRLIB_IRQMPSTATUS_CPUNR     28

#include <grlib/ambapp.h>
#include <grlib/grlib.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t GRLIB_Cpu_Index;
extern const unsigned char GRLIB_mp_irq;

extern volatile struct irqmp_regs *GRLIB_IrqCtrl_Regs;
extern struct ambapp_dev *GRLIB_IrqCtrl_Adev;
extern int GRLIB_IrqCtrl_EIrq;
extern volatile struct gptimer_regs *GRLIB_Timer_Regs;
extern struct ambapp_dev *GRLIB_Timer_Adev;
void gptimer_initialize(void);
void irqmp_initialize(void);

static inline uint32_t grlib_up_counter_frequency(void)
{
  /*
   * For simplicity, assume that the interrupt controller uses the processor
   * clock.  This is at least true on the GR740.
   */
  return ambapp_freq_get(ambapp_plb(), GRLIB_IrqCtrl_Adev);
}

extern rtems_interrupt_lock GRLIB_IrqCtrl_Lock;


static inline uint32_t grlib_get_cpu_count(
  volatile struct irqmp_regs *irqmp
)
{
  uint32_t mpstat = irqmp->mpstat;

  return ((mpstat >> GRLIB_IRQMPSTATUS_CPUNR) & 0xf)  + 1;
}

#define GRLIB_IRQCTRL_ACQUIRE( _lock_context ) \
  rtems_interrupt_lock_acquire( &GRLIB_IrqCtrl_Lock, _lock_context )

#define GRLIB_IRQCTRL_RELEASE( _lock_context ) \
  rtems_interrupt_lock_release( &GRLIB_IrqCtrl_Lock, _lock_context )

#define GRLIB_Cpu_Unmask_interrupt( _source, _cpu ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    GRLIB_IRQCTRL_ACQUIRE( &_lock_context ); \
    GRLIB_IrqCtrl_Regs->mask[_cpu]  |= (1U << (_source)); \
    GRLIB_IRQCTRL_RELEASE( &_lock_context ); \
  } while (0)

#define GRLIB_Cpu_Mask_interrupt( _source, _cpu ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    GRLIB_IRQCTRL_ACQUIRE( &_lock_context ); \
     GRLIB_IrqCtrl_Regs->mask[_cpu]  &= ~(1U << (_source)); \
    GRLIB_IRQCTRL_RELEASE( &_lock_context ); \
  } while (0)

#define GRLIB_Enable_interrupt_broadcast( _source ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    uint32_t _mask = 1U << ( _source ); \
    GRLIB_IRQCTRL_ACQUIRE( &_lock_context ); \
    GRLIB_IrqCtrl_Regs->bcast |= _mask; \
    GRLIB_IRQCTRL_RELEASE( &_lock_context ); \
  } while (0)

#define GRLIB_Disable_interrupt_broadcast( _source ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    uint32_t _mask = 1U << ( _source ); \
    GRLIB_IRQCTRL_ACQUIRE( &_lock_context ); \
    GRLIB_IrqCtrl_Regs->bcast &= ~_mask; \
    GRLIB_IRQCTRL_RELEASE( &_lock_context ); \
  } while (0)

#define BSP_Cpu_Is_interrupt_masked( _source, _cpu ) \
     (!(GRLIB_IrqCtrl_Regs->mask[_cpu] & (1U << (_source))))

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __AMBA_H__ */
