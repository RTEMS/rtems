/**
 * @file
 *
 * @ingroup ppc_exc
 *
 * @brief PowerPC Exceptions implementation.
 */

/*
 * Copyright (C) 1999 Eric Valette (valette@crf.canon.fr)
 *                    Canon Centre Recherche France.
 *
 * Derived from file "libcpu/powerpc/new-exceptions/bspsupport/vectors_init.c".
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/vectors.h>

#ifndef __SPE__
  #define GET_GPR(gpr) (gpr)
#else
  #define GET_GPR(gpr) ((int) ((gpr) >> 32))
#endif

exception_handler_t globalExceptHdl = C_exception_handler;

/* T. Straumann: provide a stack trace
 * <strauman@slac.stanford.edu>, 6/26/2001
 */
typedef struct LRFrameRec_ {
  struct LRFrameRec_ *frameLink;
  unsigned long *lr;
} LRFrameRec, *LRFrame;

#define STACK_CLAMP 50          /* in case we have a corrupted bottom */

static uint32_t ppc_exc_get_DAR_dflt(void)
{
  if (ppc_cpu_is_60x())
    return PPC_SPECIAL_PURPOSE_REGISTER(PPC_DAR);
  else
    switch (ppc_cpu_is_bookE()) {
      default:
        break;
      case PPC_BOOKE_STD:
      case PPC_BOOKE_E500:
        return PPC_SPECIAL_PURPOSE_REGISTER(DEAR_BOOKE);
      case PPC_BOOKE_405:
        return PPC_SPECIAL_PURPOSE_REGISTER(DEAR_405);
    }
  return 0xdeadbeef;
}

uint32_t (*ppc_exc_get_DAR)(void) = ppc_exc_get_DAR_dflt;

void BSP_printStackTrace(BSP_Exception_frame *excPtr)
{
  LRFrame f;
  int i;
  LRFrame sp;
  void *lr;

  printk("Stack Trace: \n  ");
  if (excPtr) {
    printk("IP: 0x%08x, ", excPtr->EXC_SRR0);
    sp = (LRFrame) GET_GPR(excPtr->GPR1);
    lr = (void *) excPtr->EXC_LR;
  } else {
    /* there's no macro for this */
    __asm__ __volatile__("mr %0, 1":"=r"(sp));
    lr = (LRFrame) ppc_link_register();
  }
  printk("LR: 0x%08x\n", lr);
  for (f = (LRFrame) sp, i = 0; f->frameLink && i < STACK_CLAMP; f = f->frameLink) {
    printk("--^ 0x%08x", (long) (f->frameLink->lr));
    if (!(++i % 5))
      printk("\n");
  }
  if (i >= STACK_CLAMP) {
    printk("Too many stack frames (stack possibly corrupted), giving up...\n");
  } else {
    if (i % 5)
      printk("\n");
  }
}

void C_exception_handler(BSP_Exception_frame *excPtr)
{
  static int nest = 0;

  int recoverable = 0;
  rtems_id id = 0;
  int synch;
  unsigned n;
  rtems_status_code sc;

  /* Catch recursion */
  nest++;

  if (nest > 2) {
    /* maybe printk() or dereferencing excPtr caused an exception;
     * die silently...
     */
    while (1);
  }

  synch = (int) excPtr->_EXC_number >= 0;
  n = excPtr->_EXC_number & 0x7fff;

  printk("Exception handler called for exception %d (0x%x)\n", n, n);
  printk("\t Next PC or Address of fault = %08x\n", excPtr->EXC_SRR0);
  printk("\t Saved MSR = %08x\n", excPtr->EXC_SRR1);

  if (nest > 1) {
    printk("Recursion in the exception handler detected; I'll spin now...\n");
    while (1);
  }

  /* Try to find out more about the context where this happened */
  printk("\t Context: ");
  if (rtems_interrupt_is_in_progress()) {
    printk("ISR");
  } else if (!_Thread_Executing) {
    printk("Initialization (_Thread_Executing not available yet)");
  } else {
    if (RTEMS_SUCCESSFUL != (sc = rtems_task_ident(RTEMS_SELF, RTEMS_LOCAL, &id))) {
      printk("Unable to determine faulting task; rtems_task_ident() returned %u", sc);
      id = 0;
    } else {
      printk("Task ID 0x%08x", id);
    }
  }
  printk("\n");

  /* Dump registers */

  printk("\t R0  = %08x", GET_GPR(excPtr->GPR0));
  if (synch) {
    printk(" R1  = %08x", GET_GPR(excPtr->GPR1));
    printk(" R2  = %08x", GET_GPR(excPtr->GPR2));
  } else {
    printk("               ");
    printk("               ");
  }
  printk(" R3  = %08x\n", GET_GPR(excPtr->GPR3));
  printk("\t R4  = %08x", GET_GPR(excPtr->GPR4));
  printk(" R5  = %08x", GET_GPR(excPtr->GPR5));
  printk(" R6  = %08x", GET_GPR(excPtr->GPR6));
  printk(" R7  = %08x\n", GET_GPR(excPtr->GPR7));
  printk("\t R8  = %08x", GET_GPR(excPtr->GPR8));
  printk(" R9  = %08x", GET_GPR(excPtr->GPR9));
  printk(" R10 = %08x", GET_GPR(excPtr->GPR10));
  printk(" R11 = %08x\n", GET_GPR(excPtr->GPR11));
  printk("\t R12 = %08x", GET_GPR(excPtr->GPR12));
  if (synch) {
    printk(" R13 = %08x", GET_GPR(excPtr->GPR13));
    printk(" R14 = %08x", GET_GPR(excPtr->GPR14));
    printk(" R15 = %08x\n", GET_GPR(excPtr->GPR15));
    printk("\t R16 = %08x", GET_GPR(excPtr->GPR16));
    printk(" R17 = %08x", GET_GPR(excPtr->GPR17));
    printk(" R18 = %08x", GET_GPR(excPtr->GPR18));
    printk(" R19 = %08x\n", GET_GPR(excPtr->GPR19));
    printk("\t R20 = %08x", GET_GPR(excPtr->GPR20));
    printk(" R21 = %08x", GET_GPR(excPtr->GPR21));
    printk(" R22 = %08x", GET_GPR(excPtr->GPR22));
    printk(" R23 = %08x\n", GET_GPR(excPtr->GPR23));
    printk("\t R24 = %08x", GET_GPR(excPtr->GPR24));
    printk(" R25 = %08x", GET_GPR(excPtr->GPR25));
    printk(" R26 = %08x", GET_GPR(excPtr->GPR26));
    printk(" R27 = %08x\n", GET_GPR(excPtr->GPR27));
    printk("\t R28 = %08x", GET_GPR(excPtr->GPR28));
    printk(" R29 = %08x", GET_GPR(excPtr->GPR29));
    printk(" R30 = %08x", GET_GPR(excPtr->GPR30));
    printk(" R31 = %08x\n", GET_GPR(excPtr->GPR31));
  } else {
    printk("\n");
  }
  printk("\t CR  = %08x\n", excPtr->EXC_CR);
  printk("\t CTR = %08x\n", excPtr->EXC_CTR);
  printk("\t XER = %08x\n", excPtr->EXC_XER);
  printk("\t LR  = %08x\n", excPtr->EXC_LR);

  /* Would be great to print DAR but unfortunately,
   * that is not portable across different CPUs.
   * AFAIK on classic PPC DAR is SPR 19, on the
   * 405 we have DEAR = SPR 0x3d5 and booE says
   * DEAR = SPR 61 :-(
   */
  if (ppc_exc_get_DAR) {
    printk("\t DAR = %08x\n", ppc_exc_get_DAR());
  }

  BSP_printStackTrace(excPtr);

  if (excPtr->_EXC_number == ASM_DEC_VECTOR)
    recoverable = 1;
  if (excPtr->_EXC_number == ASM_SYS_VECTOR)
#ifdef TEST_RAW_EXCEPTION_CODE
    recoverable = 1;
#else
    recoverable = 0;
#endif
  if (!recoverable) {
    if (id) {
      printk("Suspending faulting task (0x%08x)\n", id);
      /* Unnest here because rtems_task_suspend() never returns */
      nest--;
      rtems_task_suspend(id);
    } else {
      printk("unrecoverable exception!!! Push reset button\n");
      while (1);
    }
  } else {
    nest--;
  }
}
