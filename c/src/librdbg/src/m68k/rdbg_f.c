/*
 **************************************************************************
 *
 * Component =   
 * 
 * Synopsis  =   rdbg/m68k/rdbg_f.c
 *
 * $Id$
 *
 **************************************************************************
 */

#include <assert.h>
#include <errno.h>
#include <rdbg/reg.h>
#include <rdbg/remdeb.h>
#include <rdbg/rdbg.h>
#include <rtems/score/cpu.h>
#include <rtems/score/thread.h>

m68k_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

void CtxToRegs (const CPU_Exception_frame * ctx, xdr_regs * regs)
{
  regs->r_dreg[0] = ctx->d0;
  regs->r_dreg[1] = ctx->d1;
  regs->r_dreg[2] = ctx->d2;
  regs->r_dreg[3] = ctx->d3;
  regs->r_dreg[4] = ctx->d4;
  regs->r_dreg[5] = ctx->d5;
  regs->r_dreg[6] = ctx->d6;
  regs->r_dreg[7] = ctx->d7;

  regs->r_areg[0] = ctx->a0;
  regs->r_areg[1] = ctx->a1;
  regs->r_areg[2] = ctx->a2;
  regs->r_areg[3] = ctx->a3;
  regs->r_areg[4] = ctx->a4;
  regs->r_areg[5] = ctx->a5;
  regs->r_areg[6] = ctx->a6;
  regs->r_areg[7] = ctx->a7;

  regs->r_sr = ctx->sr;
  regs->r_pc = ctx->pc;
  regs->r_vec = ctx->vecnum;
}

  void
RegsToCtx (const xdr_regs * regs, CPU_Exception_frame * ctx)
{
  ctx->d0 = regs->r_dreg[0];
  ctx->d1 = regs->r_dreg[1];
  ctx->d2 = regs->r_dreg[2];
  ctx->d3 = regs->r_dreg[3];
  ctx->d4 = regs->r_dreg[4];
  ctx->d5 = regs->r_dreg[5];
  ctx->d6 = regs->r_dreg[6];
  ctx->d7 = regs->r_dreg[7];

  ctx->a0 = regs->r_areg[0];
  ctx->a1 = regs->r_areg[1];
  ctx->a2 = regs->r_areg[2];
  ctx->a3 = regs->r_areg[3];
  ctx->a4 = regs->r_areg[4];
  ctx->a5 = regs->r_areg[5];
  ctx->a6 = regs->r_areg[6];
  ctx->a7 = regs->r_areg[7];

  ctx->sr = regs->r_sr;
  ctx->pc = regs->r_pc;
  ctx->vecnum = regs->r_vec;
}

  void
get_ctx_thread (Thread_Control * thread, CPU_Exception_frame * ctx)
{
  /*
   * ISR stores d0-d1/a0-a1, calls _Thread_Dispatch
   * _Thread_Dispatch calls _Context_Switch == _CPU_Context_switch
   * _CPU_Context_switch stores/restores sr,d2-d7,a2-a7
   * so if my reasoning is correct, *sp points to a location in
   * _Thread_Dispatch
   */
  ctx->vecnum = 0xdeadbeef;
  ctx->sr = thread->Registers.sr;
  ctx->pc = *(unsigned32 *) thread->Registers.a7_msp;
  ctx->d0 = 0xdeadbeef;
  ctx->d1 = 0xdeadbeef;
  ctx->a0 = 0xdeadbeef;
  ctx->a1 = 0xdeadbeef;

  ctx->a2 = (unsigned32) thread->Registers.a2;
  ctx->a3 = (unsigned32) thread->Registers.a3;
  ctx->a4 = (unsigned32) thread->Registers.a4;
  ctx->a5 = (unsigned32) thread->Registers.a5;
  ctx->a6 = (unsigned32) thread->Registers.a6;
  ctx->a7 = (unsigned32) thread->Registers.a7_msp;
  ctx->d2 = thread->Registers.d2;
  ctx->d3 = thread->Registers.d3;
  ctx->d4 = thread->Registers.d4;
  ctx->d5 = thread->Registers.d5;
  ctx->d6 = thread->Registers.d6;
  ctx->d7 = thread->Registers.d7;
}

  void
set_ctx_thread (Thread_Control * thread, CPU_Exception_frame * ctx)
{
  thread->Registers.sr = ctx->sr;
  thread->Registers.d2 = ctx->d2;
  thread->Registers.d3 = ctx->d3;
  thread->Registers.d4 = ctx->d4;
  thread->Registers.d5 = ctx->d5;
  thread->Registers.d6 = ctx->d6;
  thread->Registers.d7 = ctx->d7;
  thread->Registers.a2 = (void *) ctx->a2;
  thread->Registers.a3 = (void *) ctx->a3;
  thread->Registers.a4 = (void *) ctx->a4;
  thread->Registers.a5 = (void *) ctx->a5;
  thread->Registers.a6 = (void *) ctx->a6;
  thread->Registers.a7_msp = (void *) ctx->a7;
}

  int
Single_Step (CPU_Exception_frame * ctx)
{
  if ((ctx->sr & (1 << 15)) != 0 || ExitForSingleStep != 0) {
    /*
     * Check coherency 
     */
    assert ((ctx->sr & (1 << 15)) != 0);
    assert (ExitForSingleStep != 0);
    return 0;
  }
  ctx->sr |= 1 << 15;
  ++ExitForSingleStep;
  return 0;
}

  int
CheckForSingleStep (CPU_Exception_frame * ctx)
{
  if (ExitForSingleStep) {
    ctx->sr &= ~(1 << 15);
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
  ctx->sr &= ~(1 << 15);
  ExitForSingleStep--;
}

extern rtems_isr excHandler ();

  void
connect_rdbg_exception (void)
{
  set_vector (excHandler, 9, 0);
  set_vector (excHandler, 47, 0);
  set_vector (excHandler, 36, 0);
}

void
disconnect_rdbg_exception (void)
{
}
