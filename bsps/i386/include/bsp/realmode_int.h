/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
 *
 * @brief Definitioins supporting real mode interrupt calls.
 *
 * Interface allows calling given interrupt number with content of the
 * registers defined. For passing or receiving higher amounts of the data
 * there is a buffer accessible from real mode available. Real mode pointer
 * to this buffer is passed to the interrupt in the registers.
 */

/*
 * Copyright (C) 2014  Jan Doležal (dolezj21@fel.cvut.cz)
 *                     CTU in Prague.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _REALMODE_INT_H
#define _REALMODE_INT_H

#include <rtems/score/cpu.h>
#include <stdint.h>

#ifndef ASM /* ASM */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* --- BIOS service interrupt number --- */
/* number of interrupt servicing video functions */
#define INTERRUPT_NO_VIDEO_SERVICES 0x10

/**
 * @brief Used for passing and retrieving registers content to/from real mode
 * interrupt call.
 */
typedef struct {
    uint32_t reg_eax;
    uint32_t reg_ebx;
    uint32_t reg_ecx;
    uint32_t reg_edx;
    uint32_t reg_esi;
    uint32_t reg_edi;
    uint16_t reg_ds;
    uint16_t reg_es;
    uint16_t reg_fs;
    uint16_t reg_gs;
} RTEMS_PACKED i386_realmode_interrupt_registers;

/**
 * @brief Returns buffer and its size usable with real mode interrupt call.
 *
 * Provides position to real mode buffer. It is buffer
 * accessible from real mode context - it is located below
 * address ~0x100000 in order for it to be accessible
 * This buffer is meant to be pointed to by segReg:GenPurpReg
 * and through this get bigger portion of an information to/from
 * interrupt service routine than just by using register.
 *
 * @param[out] size pointer to variable, where the size of buffer
 *             will be filled
 * @retval pointer to buffer
 */
extern void *i386_get_default_rm_buffer(uint16_t *size);

/**
 * @brief Call to real mode interrupt with specified int NO and processor
 * registers.
 *
 * This function allows calling interrupts in real mode and to set processor
 * registers as desired before interrupt call is made and to retrieve the
 * registers content after call was made.
 *
 * @param[in] interrupt_number interrupt number to be called
 * @param[in] ir pointer to structure containing registers to be passed to
 *            interrupt and to retrieve register content after call was made.
 * @retval  0 call failed (GDT too small or pagin is on)
 * @retval  1 call successful
 */
extern int i386_real_interrupt_call(
    uint8_t interrupt_number,
    i386_realmode_interrupt_registers *ir
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* _REALMODE_INT_H */
