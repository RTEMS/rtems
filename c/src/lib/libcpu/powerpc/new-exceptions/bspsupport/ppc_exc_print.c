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

#include <rtems/score/threaddispatch.h>

#ifndef __SPE__
  #define GET_GPR(gpr) (gpr)
#else
  #define GET_GPR(gpr) ((int) ((gpr) >> 32))
#endif

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
        return PPC_SPECIAL_PURPOSE_REGISTER(BOOKE_DEAR);
      case PPC_BOOKE_405:
        return PPC_SPECIAL_PURPOSE_REGISTER(PPC405_DEAR);
    }
  return 0xdeadbeef;
}

uint32_t (*ppc_exc_get_DAR)(void) = ppc_exc_get_DAR_dflt;

void BSP_printStackTrace(const BSP_Exception_frame *excPtr)
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

void _BSP_Exception_frame_print(const CPU_Exception_frame *excPtr)
{
  const Thread_Control *executing = _Thread_Executing;
  bool synch = (int) excPtr->_EXC_number >= 0;
  unsigned n = excPtr->_EXC_number & 0x7fff;

  printk("exception vector %d (0x%x)\n", n, n);
  printk("  next PC or address of fault = 0x%08x\n", excPtr->EXC_SRR0);
  printk("  saved MSR = 0x%08x\n", excPtr->EXC_SRR1);

  /* Try to find out more about the context where this happened */
  printk(
    "  context = %s, ISR nest level = %u\n",
    _ISR_Nest_level == 0 ? "task" : "interrupt",
    _ISR_Nest_level
  );
  printk(
    "  thread dispatch disable level = %u\n",
    _Thread_Dispatch_disable_level
  );

  /* Dump registers */

  printk("  R0  = 0x%08x", GET_GPR(excPtr->GPR0));
  if (synch) {
    printk(" R1  = 0x%08x", GET_GPR(excPtr->GPR1));
    printk(" R2  = 0x%08x", GET_GPR(excPtr->GPR2));
  } else {
    printk("               ");
    printk("               ");
  }
  printk(" R3  = 0x%08x\n", GET_GPR(excPtr->GPR3));
  printk("  R4  = 0x%08x", GET_GPR(excPtr->GPR4));
  printk(" R5  = 0x%08x", GET_GPR(excPtr->GPR5));
  printk(" R6  = 0x%08x", GET_GPR(excPtr->GPR6));
  printk(" R7  = 0x%08x\n", GET_GPR(excPtr->GPR7));
  printk("  R8  = 0x%08x", GET_GPR(excPtr->GPR8));
  printk(" R9  = 0x%08x", GET_GPR(excPtr->GPR9));
  printk(" R10 = 0x%08x", GET_GPR(excPtr->GPR10));
  printk(" R11 = 0x%08x\n", GET_GPR(excPtr->GPR11));
  printk("  R12 = 0x%08x", GET_GPR(excPtr->GPR12));
  if (synch) {
    printk(" R13 = 0x%08x", GET_GPR(excPtr->GPR13));
    printk(" R14 = 0x%08x", GET_GPR(excPtr->GPR14));
    printk(" R15 = 0x%08x\n", GET_GPR(excPtr->GPR15));
    printk("  R16 = 0x%08x", GET_GPR(excPtr->GPR16));
    printk(" R17 = 0x%08x", GET_GPR(excPtr->GPR17));
    printk(" R18 = 0x%08x", GET_GPR(excPtr->GPR18));
    printk(" R19 = 0x%08x\n", GET_GPR(excPtr->GPR19));
    printk("  R20 = 0x%08x", GET_GPR(excPtr->GPR20));
    printk(" R21 = 0x%08x", GET_GPR(excPtr->GPR21));
    printk(" R22 = 0x%08x", GET_GPR(excPtr->GPR22));
    printk(" R23 = 0x%08x\n", GET_GPR(excPtr->GPR23));
    printk("  R24 = 0x%08x", GET_GPR(excPtr->GPR24));
    printk(" R25 = 0x%08x", GET_GPR(excPtr->GPR25));
    printk(" R26 = 0x%08x", GET_GPR(excPtr->GPR26));
    printk(" R27 = 0x%08x\n", GET_GPR(excPtr->GPR27));
    printk("  R28 = 0x%08x", GET_GPR(excPtr->GPR28));
    printk(" R29 = 0x%08x", GET_GPR(excPtr->GPR29));
    printk(" R30 = 0x%08x", GET_GPR(excPtr->GPR30));
    printk(" R31 = 0x%08x\n", GET_GPR(excPtr->GPR31));
  } else {
    printk("\n");
  }
  printk("  CR  = 0x%08x\n", excPtr->EXC_CR);
  printk("  CTR = 0x%08x\n", excPtr->EXC_CTR);
  printk("  XER = 0x%08x\n", excPtr->EXC_XER);
  printk("  LR  = 0x%08x\n", excPtr->EXC_LR);

  /* Would be great to print DAR but unfortunately,
   * that is not portable across different CPUs.
   * AFAIK on classic PPC DAR is SPR 19, on the
   * 405 we have DEAR = SPR 0x3d5 and bookE says
   * DEAR = SPR 61 :-(
   */
  if (ppc_exc_get_DAR != NULL) {
    char* reg = ppc_cpu_is_60x() ? " DAR" : "DEAR";
    printk(" %s = 0x%08x\n", reg, ppc_exc_get_DAR());
  }
  if (ppc_cpu_is_bookE()) {
    unsigned esr, mcsr;
    if (ppc_cpu_is_bookE() == PPC_BOOKE_405) {
      esr  = PPC_SPECIAL_PURPOSE_REGISTER(PPC405_ESR);
      mcsr = PPC_SPECIAL_PURPOSE_REGISTER(PPC405_MCSR);
    } else {
      esr  = PPC_SPECIAL_PURPOSE_REGISTER(BOOKE_ESR);
      mcsr = PPC_SPECIAL_PURPOSE_REGISTER(BOOKE_MCSR);
    }
    printk("  ESR = 0x%08x\n", esr);
    printk(" MCSR = 0x%08x\n", mcsr);
  }

  if (executing != NULL) {
    const char *name = (const char *) &executing->Object.name;

    printk(
      "  executing thread ID = 0x%08x, name = %c%c%c%c\n",
      executing->Object.id,
      name [0],
      name [1],
      name [2],
      name [3]
    );
  } else {
    printk("  executing thread pointer is NULL");
  }

  BSP_printStackTrace(excPtr);
}
