/*
 **************************************************************************
 *
 * Component =   
 * 
 * Synopsis  =   rdbg/i386/rdbg_f.c
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

  void
CtxToRegs (const CPU_Exception_frame * ctx, xdr_regs * regs)
{
  regs->tabreg[GS] = 0;
  regs->tabreg[FS] = 0;
  regs->tabreg[ES] = 0;
  regs->tabreg[DS] = 0;
  regs->tabreg[EDI] = ctx->edi;
  regs->tabreg[ESI] = ctx->esi;
  regs->tabreg[EBP] = ctx->ebp;
  regs->tabreg[ESP] = ctx->esp0;
  regs->tabreg[EBX] = ctx->ebx;
  regs->tabreg[EDX] = ctx->edx;
  regs->tabreg[ECX] = ctx->ecx;
  regs->tabreg[EAX] = ctx->eax;
  regs->tabreg[TRAPNO] = ctx->idtIndex;
  regs->tabreg[ERR] = ctx->faultCode;
  regs->tabreg[EIP] = ctx->eip;
  regs->tabreg[CS] = ctx->cs & 0xFFFF;
  regs->tabreg[EFL] = ctx->eflags;
}

  void
RegsToCtx (const xdr_regs * regs, CPU_Exception_frame * ctx)
{
  ctx->edi = regs->tabreg[EDI];
  ctx->esi = regs->tabreg[ESI];
  ctx->ebp = regs->tabreg[EBP];
  ctx->esp0 = regs->tabreg[ESP];
  ctx->ebx = regs->tabreg[EBX];
  ctx->edx = regs->tabreg[EDX];
  ctx->ecx = regs->tabreg[ECX];
  ctx->eax = regs->tabreg[EAX];
  ctx->idtIndex = regs->tabreg[TRAPNO];
  ctx->faultCode = regs->tabreg[ERR];
  ctx->eip = regs->tabreg[EIP];
  ctx->cs = regs->tabreg[CS];
  ctx->eflags = regs->tabreg[EFL];
}

  void
get_ctx_thread (Thread_Control * thread, CPU_Exception_frame * ctx)
{
  ctx->edi = thread->Registers.edi;
  ctx->esi = thread->Registers.esi;
  ctx->ebp = (unsigned32) (thread->Registers.ebp);
  ctx->esp0 = (unsigned32) (thread->Registers.esp);
  ctx->ebx = thread->Registers.ebx;
  ctx->edx = 0;
  ctx->ecx = 0;
  ctx->eax = 0;
  ctx->idtIndex = 0;
  ctx->faultCode = 0;
  ctx->eip = *(unsigned int *) (thread->Registers.esp);
  ctx->cs = 0;
  ctx->eflags = thread->Registers.eflags;
}

  void
set_ctx_thread (Thread_Control * thread, CPU_Exception_frame * ctx)
{
  thread->Registers.edi = ctx->edi;
  thread->Registers.esi = ctx->esi;
  thread->Registers.ebp = (void *) (ctx->ebp);
  thread->Registers.esp = (void *) (ctx->esp0);
  thread->Registers.ebx = ctx->ebx;
  thread->Registers.eflags = ctx->eflags;
}

  int
Single_Step (CPU_Exception_frame * ctx)
{
  /*
   * Check if not already set 
   */
  if ((ctx->eflags & EFLAGS_TF) != 0 || ExitForSingleStep != 0) {
    /*
     * Check coherency 
     */
    assert ((ctx->eflags & EFLAGS_TF) != 0);
    assert (ExitForSingleStep != 0);
    return 0;
  }
  ctx->eflags |= EFLAGS_TF;     /* eflags */
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
    ctx->eflags &= ~EFLAGS_TF;
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
  ctx->eflags &= ~EFLAGS_TF;
  ExitForSingleStep--;
}

static cpuExcHandlerType old_currentExcHandler;
extern void rtems_exception_prologue_50 ();

  void
connect_rdbg_exception ()
{
  interrupt_gate_descriptor *currentIdtEntry;
  unsigned limit;
  unsigned level;

  /*
   *  Connect the Exception used to debug
   */
  i386_get_info_from_IDTR (&currentIdtEntry, &limit);

  _CPU_ISR_Disable (level);
  create_interrupt_gate_descriptor (&currentIdtEntry[50],
                                    rtems_exception_prologue_50);
  _CPU_ISR_Enable (level);

  if (_currentExcHandler != BreakPointExcHdl) {
    old_currentExcHandler = _currentExcHandler;
    _currentExcHandler = BreakPointExcHdl;
  }
}

  void
disconnect_rdbg_exception ()
{
  if (_currentExcHandler == BreakPointExcHdl) {
    _currentExcHandler = old_currentExcHandler;
  }
}
