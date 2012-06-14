/*
 *  This file sets up basic CPU dependency settings based on
 *  compiler settings.  For example, it can determine if
 *  floating point is available.  This particular implementation
 *  is specified to the Renesas v850 port.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_V850_H
#define _RTEMS_SCORE_V850_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the NO CPU family.
 *  It does this by setting variables to indicate which
 *  implementation dependent features are present in a particular
 *  member of the family.
 *
 *  This is a good place to list all the known CPU models
 *  that this port supports and which RTEMS CPU model they correspond
 *  to.
 */

#if defined(rtems_multilib)
/*
 *  Figure out all CPU Model Feature Flags based upon compiler
 *  predefines.
 */
#define CPU_MODEL_NAME  "rtems_multilib"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 0

#elif defined(__v850e2v3__)
#define CPU_MODEL_NAME  "v850e2v3"
#define V850_HAS_FPU 1
#define V850_HAS_BYTE_SWAP_INSTRUCTION 1

#elif defined(__v850e2__)
#define CPU_MODEL_NAME  "v850e2"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 1

#elif defined(__v850es__)
#define CPU_MODEL_NAME  "v850es"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 1

#elif defined(__v850e1__)
#define CPU_MODEL_NAME  "v850e1"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 1

#elif defined(__v850e__)
#define CPU_MODEL_NAME  "v850e"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 1

#else
#define CPU_MODEL_NAME  "v850"
#define V850_HAS_FPU 0
#define V850_HAS_BYTE_SWAP_INSTRUCTION 0

#endif

/*
 *  Define the name of the CPU family.
 */
#define CPU_NAME "v850 CPU"

/*
 *  Method to set the Program Status Word (PSW)
 */
#define v850_set_psw( _psw ) \
    __asm__ __volatile__( "ldsr %0, psw" : : "r" (_psw) )

/*
 *  Method to obtain the Program Status Word (PSW)
 */
#define v850_get_psw( _psw ) \
    __asm__ __volatile__( "stsr psw, %0" : "=&r" (_psw) )

/*
 *  Masks and bits in the Program Status Word (PSW)
 */
#define V850_PSW_ZERO_MASK                       0x01
#define V850_PSW_IS_ZERO                         0x01
#define V850_PSW_IS_NOT                          0x00

#define V850_PSW_SIGN_MASK                       0x02
#define V850_PSW_SIGN_IS_NEGATIVE                0x02
#define V850_PSW_SIGN_IS_ZERO_OR_POSITIVE        0x00

#define V850_PSW_OVERFLOW_MASK                   0x02
#define V850_PSW_OVERFLOW_OCCURRED               0x02
#define V850_PSW_OVERFLOW_DID_NOT_OCCUR          0x00

#define V850_PSW_CARRY_OR_BORROW_MASK            0x04
#define V850_PSW_CARRY_OR_BORROW_OCCURRED        0x04
#define V850_PSW_CARRY_OR_BORROW_DID_NOT_OCCUR   0x00

#define V850_PSW_SATURATION_MASK                 0x10
#define V850_PSW_SATURATION_OCCURRED             0x10
#define V850_PSW_SATURATION_DID_NOT_OCCUR        0x00

#define V850_PSW_INTERRUPT_DISABLE_MASK          0x20
#define V850_PSW_INTERRUPT_DISABLE               0x20
#define V850_PSW_INTERRUPT_ENABLE                0x00

#define V850_PSW_EXCEPTION_IN_PROCESS_MASK       0x40
#define V850_PSW_EXCEPTION_IN_PROCESS            0x40
#define V850_PSW_EXCEPTION_NOT_IN_PROCESS        0x00

#define V850_PSW_NMI_IN_PROCESS_MASK             0x80
#define V850_PSW_NMI_IN_PROCESS                  0x80
#define V850_PSW_NMI_NOT_IN_PROCESS              0x00

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_V850_H */
