/*
 **************************************************************************
 *
 * Component =   
 * 
 * Synopsis  =   rdbg/i386/excep.c
 *
 **************************************************************************
 */

#include <rtems.h>
#include <rtems/error.h>
#include <rdbg/rdbg_f.h>
#include <assert.h>
#include <errno.h>
#include <rdbg/rdbg.h>
#include <rdbg/servrpc.h>


extern rtems_id serializeSemId;
extern rtems_id wakeupEventSemId;


unsigned int NbExceptCtx;
volatile unsigned int NbSerializedCtx;
Exception_context *FirstCtx = NULL;
Exception_context *LastCtx = NULL;

CPU_Exception_frame SavedContext;


/********* Save an exception context at the end of a list *****/

int PushExceptCtx ( Objects_Id Id, Objects_Id semId, CPU_Exception_frame *ctx ) {

  Exception_context *SaveCtx;

  SaveCtx = (Exception_context *)malloc(sizeof(Exception_context));
  if (SaveCtx == NULL)
    rtems_panic("Can't allocate memory to save Exception context");
  
  SaveCtx->id = Id;
  SaveCtx->ctx = ctx;
  SaveCtx->semaphoreId = semId;
  SaveCtx->previous = NULL;
  SaveCtx->next = NULL;
  
  if (FirstCtx == NULL){  /* initialization */
    FirstCtx = SaveCtx;
    LastCtx  = SaveCtx;
    NbExceptCtx = 1;
  }
  else {
    NbExceptCtx ++;
    LastCtx->next = SaveCtx;
    SaveCtx->previous = LastCtx;
    LastCtx = SaveCtx;
  }
  return 0;
}

/********* Save an temporary exception context in a ******/
/********* global variable                          ******/

int PushSavedExceptCtx ( Objects_Id Id, CPU_Exception_frame *ctx ) {

  memcpy (&(SavedContext), ctx, sizeof(CPU_Exception_frame));
  return 0;
}


/****** Remove the context of the specified Id thread *********/
/****** If Id = -1, then return the first context     *********/


int PopExceptCtx ( Objects_Id Id ) {

  Exception_context *ExtractCtx;
  
  if (FirstCtx == NULL) return -1;
  
  if  (Id == -1) {
    ExtractCtx = LastCtx;
    LastCtx = LastCtx->previous;
    free(ExtractCtx);
    NbExceptCtx --;
    return 0;
  }
  
  ExtractCtx = LastCtx;
  
  while (ExtractCtx->id != Id && ExtractCtx != NULL) {
    ExtractCtx = ExtractCtx->previous;
  }

  if (ExtractCtx == NULL)
    return -1;

  if ( ExtractCtx->previous != NULL)
    (ExtractCtx->previous)->next = ExtractCtx->next;

  if ( ExtractCtx->next != NULL)
    (ExtractCtx->next)->previous = ExtractCtx->previous;

  if (ExtractCtx == FirstCtx)
    FirstCtx = FirstCtx->next;
  else
  if (ExtractCtx == LastCtx)
    LastCtx = LastCtx->previous;
  
  free(ExtractCtx);
  NbExceptCtx --;
  return 0;
}
  
/****** Return the context of the specified Id thread *********/
/****** If Id = -1, then return the first context     *********/


Exception_context *GetExceptCtx ( Objects_Id Id ) {

  Exception_context *ExtractCtx;

  if (FirstCtx == NULL) return NULL;
  
  if  (Id == -1) {
    return LastCtx;
  }
  
  ExtractCtx = LastCtx;

  while (ExtractCtx->id != Id && ExtractCtx != NULL) {
    ExtractCtx = ExtractCtx->previous;
  }

  if (ExtractCtx == NULL)
    return NULL;

  return ExtractCtx;
}
  
/*----- Breakpoint Exception management -----*/

    /*
     *  Handler for Breakpoint Exceptions :
     *  software breakpoints.
     */

void
BreakPointExcHdl(CPU_Exception_frame *ctx) 
{
  rtems_status_code status;
  rtems_id continueSemId;

  if ( (justSaveContext) && (ctx->idtIndex == I386_EXCEPTION_ENTER_RDBG) ) {
    PushSavedExceptCtx (_Thread_Executing->Object.id, ctx);
    justSaveContext = 0;
  }
  else {
    if (ctx->idtIndex != I386_EXCEPTION_DEBUG){
      NbSerializedCtx++;
      rtems_semaphore_obtain(serializeSemId, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
      NbSerializedCtx--;
    }
      
    currentTargetThread = _Thread_Executing->Object.id;

#ifdef DDEBUG
    printk("----------------------------------------------------------\n");
    printk("Exception %d caught at PC %x by thread %d\n",
	   ctx->idtIndex,
	   ctx->eip,
	   _Thread_Executing->Object.id);
    printk("----------------------------------------------------------\n");
    printk("Processor execution context at time of the fault was  :\n");
    printk("----------------------------------------------------------\n");
    printk(" EAX = %x	EBX = %x	ECX = %x	EDX = %x\n",
	   ctx->eax, ctx->ebx, ctx->ecx, ctx->edx);
    printk(" ESI = %x	EDI = %x	EBP = %x	ESP = %x\n",
	   ctx->esi, ctx->edi, ctx->ebp, ctx->esp0);
    printk("----------------------------------------------------------\n");
    printk("Error code pushed by processor itself (if not 0) = %x\n",
	   ctx->faultCode);
    printk("----------------------------------------------------------\n\n");
#endif

    status = rtems_semaphore_create (rtems_build_name('D', 'B', 'G', 'c'),
				     0,
				     RTEMS_FIFO |
				     RTEMS_COUNTING_SEMAPHORE |
				     RTEMS_NO_INHERIT_PRIORITY |
				     RTEMS_NO_PRIORITY_CEILING |
				     RTEMS_LOCAL,
				     0,
				     &continueSemId);
    if (status != RTEMS_SUCCESSFUL)
      rtems_panic ("Can't create continue semaphore: `%s'\n",rtems_status_text(status));

    PushExceptCtx (_Thread_Executing->Object.id, continueSemId, ctx);
  
    switch (ctx->idtIndex){
    case I386_EXCEPTION_DEBUG:
      DPRINTF((" DEBUG EXCEPTION !!!\n"));
      ctx->eflags &= ~EFLAGS_TF;
      ExitForSingleStep-- ;
      rtems_semaphore_release( wakeupEventSemId );
    break;

    case I386_EXCEPTION_BREAKPOINT:
      DPRINTF((" BREAKPOINT EXCEPTION !!!\n"));
      rtems_semaphore_release( wakeupEventSemId );
    break;

    case I386_EXCEPTION_ENTER_RDBG:
      DPRINTF((" ENTER RDBG !!!\n"));
      rtems_semaphore_release( wakeupEventSemId );
      break;

    default:
      DPRINTF((" OTHER EXCEPTION !!!\n"));
      rtems_semaphore_release( wakeupEventSemId );
      break;
    }

    rtems_semaphore_obtain(continueSemId, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    
    PopExceptCtx (_Thread_Executing->Object.id);
    rtems_semaphore_delete(continueSemId);
  }
}



