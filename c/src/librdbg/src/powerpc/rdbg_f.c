/*
 **************************************************************************
 *
 * Component =   
 * 
 * Synopsis  =   rdbg/powerpc/rdbg_f.c
 *
 * $Id$
 *
 **************************************************************************
 */

#include <rtems/score/cpu.h>
#include <assert.h>
#include <errno.h>
#include <rdbg/reg.h>
#include <rdbg/remdeb.h>
#include <rdbg/rdbg.h>
#include <rtems/score/thread.h>

  void
CtxToRegs (const CPU_Exception_frame * ctx, xdr_regs * regs)
{
  *((CPU_Exception_frame *) regs) = *ctx;
}

  void
RegsToCtx (const xdr_regs * regs, CPU_Exception_frame * ctx)
{
  *ctx = *((CPU_Exception_frame *) regs);
}

  void
get_ctx_thread (Thread_Control * thread, CPU_Exception_frame * ctx)
{
  unsigned int *ptr;
  unsigned int i;

  ctx->EXC_SRR0 = thread->Registers.pc;
  ctx->EXC_SRR1 = thread->Registers.msr;
  ctx->_EXC_number = 0xdeadbeef;

  ctx->GPR1 = thread->Registers.gpr1;
  ctx->GPR2 = thread->Registers.gpr2;
  /*
   * Fill with dummy values...
   */
  ptr = &ctx->GPR3;
  for (i = 0; i < 10; i++)
    ptr[i] = 0xdeadbeef;

  ctx->GPR13 = thread->Registers.gpr13;
  ctx->GPR14 = thread->Registers.gpr14;
  ctx->GPR15 = thread->Registers.gpr15;
  ctx->GPR16 = thread->Registers.gpr16;
  ctx->GPR17 = thread->Registers.gpr17;
  ctx->GPR18 = thread->Registers.gpr18;
  ctx->GPR19 = thread->Registers.gpr19;
  ctx->GPR20 = thread->Registers.gpr20;
  ctx->GPR21 = thread->Registers.gpr21;
  ctx->GPR22 = thread->Registers.gpr22;
  ctx->GPR23 = thread->Registers.gpr23;
  ctx->GPR24 = thread->Registers.gpr24;
  ctx->GPR25 = thread->Registers.gpr25;
  ctx->GPR26 = thread->Registers.gpr26;
  ctx->GPR27 = thread->Registers.gpr27;
  ctx->GPR28 = thread->Registers.gpr28;
  ctx->GPR29 = thread->Registers.gpr29;
  ctx->GPR30 = thread->Registers.gpr30;
  ctx->GPR31 = thread->Registers.gpr31;
  ctx->EXC_CR = thread->Registers.cr;
  ctx->EXC_CTR = 0xdeadbeef;
  ctx->EXC_XER = 0xdeadbeef;
  ctx->EXC_LR = 0xdeadbeef;
  ctx->EXC_MSR = 0xdeadbeef;
  ctx->EXC_DAR = 0xdeadbeef;
}

  void
set_ctx_thread (Thread_Control * thread, CPU_Exception_frame * ctx)
{
  thread->Registers.gpr1 = ctx->GPR1;
  thread->Registers.gpr2 = ctx->GPR2;

  thread->Registers.gpr13 = ctx->GPR13;
  thread->Registers.gpr14 = ctx->GPR14;
  thread->Registers.gpr15 = ctx->GPR15;
  thread->Registers.gpr16 = ctx->GPR16;
  thread->Registers.gpr17 = ctx->GPR17;
  thread->Registers.gpr18 = ctx->GPR18;
  thread->Registers.gpr19 = ctx->GPR19;
  thread->Registers.gpr20 = ctx->GPR20;
  thread->Registers.gpr21 = ctx->GPR21;
  thread->Registers.gpr22 = ctx->GPR22;
  thread->Registers.gpr23 = ctx->GPR23;
  thread->Registers.gpr24 = ctx->GPR24;
  thread->Registers.gpr25 = ctx->GPR25;
  thread->Registers.gpr26 = ctx->GPR26;
  thread->Registers.gpr27 = ctx->GPR27;
  thread->Registers.gpr28 = ctx->GPR28;
  thread->Registers.gpr29 = ctx->GPR29;
  thread->Registers.gpr30 = ctx->GPR30;
  thread->Registers.gpr31 = ctx->GPR31;
  thread->Registers.cr = ctx->EXC_CR;
  thread->Registers.pc = ctx->EXC_SRR0;
  thread->Registers.msr = ctx->EXC_SRR1;
}

  int
Single_Step (CPU_Exception_frame * ctx)
{
  if ((ctx->EXC_SRR1 & MSR_SE) != 0 || ExitForSingleStep != 0) {
    /*
     * Check coherency 
     */
    assert ((ctx->EXC_SRR1 & MSR_SE) != 0);
    assert (ExitForSingleStep != 0);
    return 0;
  }
  ctx->EXC_SRR1 |= MSR_SE;
  ++ExitForSingleStep;
  return 0;
}

  int
CheckForSingleStep (CPU_Exception_frame * ctx)
{
  if (ExitForSingleStep) {
    /*
     *  This functions can be called both from
     *  INT1 and INT3 handlers. In case it is
     *  called from INT3, need to clear TF.
     */
    ctx->EXC_SRR1 &= ~MSR_SE;
    ExitForSingleStep = 0;
    return 1;
  }
  return 0;
}

  void
CancelSingleStep (CPU_Exception_frame * ctx)
{
  /*
   * Cancel scheduled SS 
   */
  ctx->EXC_SRR1 &= ~MSR_SE;
  ExitForSingleStep--;
}

static cpuExcHandlerType oldExcHandler;

  void
connect_rdbg_exception ()
{
  /*
   * test if rdbg already connected its exception handler
   */
  if (globalExceptHdl != BreakPointExcHdl) {
    oldExcHandler = globalExceptHdl;
    globalExceptHdl = BreakPointExcHdl;
  }
}

  void
disconnect_rdbg_exception ()
{
  /*
   * test if current execption handler is rdbg's one
   * and restore the original one in this case.
   */
  if (globalExceptHdl == BreakPointExcHdl) {
    globalExceptHdl = oldExcHandler;
  }
}
