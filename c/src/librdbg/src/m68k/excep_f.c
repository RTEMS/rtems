/*
 **************************************************************************
 *
 * Component =   
 * 
 * Synopsis  =   rdbg/m68k/excep_f.c
 *
 * $Id$
 *
 **************************************************************************
 */

#include <rtems.h>
#include <rtems/error.h>
#include <assert.h>
#include <errno.h>
#include <rdbg/rdbg.h>
#include <rdbg/servrpc.h>

  int
ExcepToSig (Exception_context * ctx)
{
  int excep = getExcNum (ctx);

  switch (excep) {
  case 2:
    return 10;
    break;                      /* bus error           */
  case 3:
    return 10;
    break;                      /* address error       */
  case 4:
    return 4;
    break;                      /* illegal instruction */
  case 5:
    return 8;
    break;                      /* zero divide         */
  case 6:
    return 8;
    break;                      /* chk instruction     */
  case 7:
    return 8;
    break;                      /* trapv instruction   */
  case 8:
    return 11;
    break;                      /* privilege violation */
  case 9:
    return 5;
    break;                      /* trace trap          */
  case 10:
    return 4;
    break;                      /* line 1010 emulator  */
  case 11:
    return 4;
    break;                      /* line 1111 emulator  */

    /*
     * Coprocessor protocol violation.  Using a standard MMU or FPU
     * this cannot be triggered by software.  Call it a SIGBUS.  
     */
  case 13:
    return 10;
    break;

  case 31:
    return 2;
    break;                      /* interrupt           */
  case 33:
    return 5;
    break;                      /* monitor breakpoint  */
  case 34:
    return 2;
    break;                      /* lets use this for SCC1 interrupt */
  case 35:
    return 5;
    break;                      /* rdbg breakpoint  */
  case 36:
    return 2;
    break;                      /* enter RDBG */
    /*
     * This is a trap #8 instruction.  Apparently it is someone's software
     * convention for some sort of SIGFPE condition.  Whose?  How many
     * people are being screwed by having this code the way it is?
     * Is there a clean solution?  
     */
  case 40:
    return 8;
    break;                      /* floating point err  */

  case 47:
    return 5;
    break;                      /* rdbg breakpoint  */

  case 48:
    return 8;
    break;                      /* floating point err  */
  case 49:
    return 8;
    break;                      /* floating point err  */
  case 50:
    return 8;
    break;                      /* zero divide         */
  case 51:
    return 8;
    break;                      /* underflow           */
  case 52:
    return 8;
    break;                      /* operand error       */
  case 53:
    return 8;
    break;                      /* overflow            */
  case 54:
    return 8;
    break;                      /* NAN                 */
  default:
    return 7;                   /* "software generated" */
  }
  return SIGKILL;
}

/*----- Breakpoint Exception management -----*/

    /*
     *  Handler for Breakpoint Exceptions :
     *  software breakpoints.
     */

  void
BreakPointExcHdl (CPU_Exception_frame * ctx)
{
  rtems_status_code status;
  rtems_id continueSemId;

  connect_rdbg_exception ();    /* monitor stub changes trace vector */
  if ((justSaveContext) && (ctx->vecnum == 47)) {   /* break */
    PushSavedExceptCtx (_Thread_Executing->Object.id, ctx);
    justSaveContext = 0;
  } else {
    if (ctx->vecnum != 9) {     /* trace */
      NbSerializedCtx++;
      rtems_semaphore_obtain (serializeSemId, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      NbSerializedCtx--;
    }

    currentTargetThread = _Thread_Executing->Object.id;

#ifdef DDEBUG
    printk ("----------------------------------------------------------\n");
    printk ("Exception %d caught at PC %x by thread %d\n",
            ctx->vecnum, ctx->pc, _Thread_Executing->Object.id);
    printk ("----------------------------------------------------------\n");
    printk ("Processor execution context at time of the fault was  :\n");
    printk ("----------------------------------------------------------\n");
    printk ("\t A0 = %x\n", ctx->a0);
    printk ("\t A1 = %x\n", ctx->a1);
    printk ("\t A2 = %x\n", ctx->a2);
    printk ("\t A3 = %x\n", ctx->a3);
    printk ("\t A4 = %x\n", ctx->a4);
    printk ("\t A5 = %x\n", ctx->a5);
    printk ("\t A6 = %x\n", ctx->a6);
    printk ("\t A7 = %x\n", ctx->a7);
    printk ("\t D0 = %x\n", ctx->d0);
    printk ("\t D1 = %x\n", ctx->d1);
    printk ("\t D2 = %x\n", ctx->d2);
    printk ("\t D3 = %x\n", ctx->d3);
    printk ("\t D4 = %x\n", ctx->d4);
    printk ("\t D5 = %x\n", ctx->d5);
    printk ("\t D6 = %x\n", ctx->d6);
    printk ("\t D7 = %x\n", ctx->d7);
    printk ("\t SR = %x\n", ctx->sr);
#endif

    status = rtems_semaphore_create (rtems_build_name ('D', 'B', 'G', 'c'),
                                     0,
                                     RTEMS_FIFO |
                                     RTEMS_COUNTING_SEMAPHORE |
                                     RTEMS_NO_INHERIT_PRIORITY |
                                     RTEMS_NO_PRIORITY_CEILING |
                                     RTEMS_LOCAL, 0, &continueSemId);
    if (status != RTEMS_SUCCESSFUL)
      rtems_panic ("Can't create continue semaphore: `%s'\n",
                   rtems_status_text (status));

    PushExceptCtx (_Thread_Executing->Object.id, continueSemId, ctx);

    switch (ctx->vecnum) {
    case 9:                    /* trace */
      DPRINTF ((" TRACE EXCEPTION !!!\n"));
      ctx->sr &= ~(1 << 15);
      ExitForSingleStep--;
      rtems_semaphore_release (wakeupEventSemId);
      break;

    case 47:                   /* trap #15 */
      DPRINTF ((" BREAKPOINT EXCEPTION !!!\n"));
      rtems_semaphore_release (wakeupEventSemId);
      break;

    case 36:                   /* trap #4 */
      DPRINTF ((" ENTER RDBG !!!\n"));
      rtems_semaphore_release (wakeupEventSemId);
      break;

    default:
      DPRINTF ((" OTHER EXCEPTION !!!\n"));
      rtems_semaphore_release (wakeupEventSemId);
      break;
    }

    rtems_semaphore_obtain (continueSemId, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

    PopExceptCtx (_Thread_Executing->Object.id);
    rtems_semaphore_delete (continueSemId);
  }

  connect_rdbg_exception ();    /* monitor stub changes trace vector */
}
