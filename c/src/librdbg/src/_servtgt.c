/*
 ============================================================================
 _SERVTGT 
 ============================================================================
*/


#include <string.h>		
#include <rtems.h>
#include <rtems/error.h>

#include <rdbg/rdbg.h>
#include <rdbg/rdbg_f.h>
#include <rdbg/servrpc.h>	
#include <errno.h>
#include <sys/socket.h>		
#include <assert.h>
#include <rtems/score/cpu.h>

extern void rtems_exception_prologue_50();


#ifdef DDEBUG
#define Ptrace	TgtDbgPtrace
#else
#define Ptrace	TgtRealPtrace
#endif

extern int errno;

rtems_id eventTaskId;
rtems_id serializeSemId;
rtems_id wakeupEventSemId;

CPU_Exception_frame Idle_frame;

cpuExcHandlerType old_currentExcHandler;

/* -----------------------------------------------------------------
   TgtRealPtrace - lowest level ptrace() wrapper
   ----------------------------------------------------------------- */

    int
TgtRealPtrace(int req, PID aid, char* addr, int d, void* addr2)
{
    return ptrace(req, aid, addr, d, addr2);
}


/* -----------------------------------------------------------------
   Maping of hardware exceptions into Unix-like signal numbers.
   It is identical to the one used by the PM and the AM.
   ----------------------------------------------------------------- */

    int
ExcepToSig (int excep)
{
    switch (excep) {
      
    case I386_EXCEPTION_MATH_COPROC_UNAVAIL: 
    case I386_EXCEPTION_I386_COPROC_SEG_ERR: 
    case I386_EXCEPTION_FLOAT_ERROR: 
    case I386_EXCEPTION_BOUND: 
	return SIGFPE;

    case I386_EXCEPTION_DEBUG: 
    case I386_EXCEPTION_BREAKPOINT: 
    case I386_EXCEPTION_ENTER_RDBG: 
	return SIGTRAP;

    case I386_EXCEPTION_OVERFLOW: 
    case I386_EXCEPTION_DIVIDE_BY_ZERO:
    case I386_EXCEPTION_ILLEGAL_INSTR:  
	return SIGILL;

    case I386_EXCEPTION_SEGMENT_NOT_PRESENT:
    case I386_EXCEPTION_STACK_SEGMENT_FAULT:
    case I386_EXCEPTION_GENERAL_PROT_ERR: 
    case I386_EXCEPTION_PAGE_FAULT: 
	return SIGSEGV;

    default:
	break;
    }
    return SIGKILL;
}

/* -----------------------------------------------------------------------
   TgtChange() is called when the system stops.
   It informs the generic layers must be informed of
   that fact. 
   ----------------------------------------------------------------------- */

    static int
TgtChange (PID pid, CPU_Exception_frame* ctx, int status)
{

    if (TgtHandleChildChange (pid, &status, NULL, ctx)) {
      TgtNotifyWaitChange (pid, status, -1);
    }

    return 0;
}

/* -----------------------------------------------------------------------
   eventTask
   ----------------------------------------------------------------------- */

rtems_task eventTask( rtems_task_argument pid)
{
  Exception_context *ctx;

  DPRINTF (("event task: pid %d\n", pid));

  
  /*
   * we spend all our time waiting for a semaphore.
   * If wait change, we send info
   */
  
  for (;;){	
    DPRINTF (("Event Task: wait event\n"));
    rtems_semaphore_obtain(wakeupEventSemId, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    DPRINTF (("Event Task: wake up !!!!!!!!!!!!!\n"));
    
    errno = 0;
    ctx = GetExceptCtx(currentTargetThread);

    CheckForSingleStep(ctx->ctx);

    TgtChange(pid, ctx->ctx,STS_MAKESIG(ExcepToSig(ctx->ctx->idtIndex)));
      
  }
}

/* -------------------------------------------------------------------
   MyThreadIdle -

   This task is used to initiate the exception mechanism:
   It calls the enterDebug function with justSaveContext=1
   only to push a first valid context in the list
   ---------------------------------------------------------------------*/

rtems_task MyThreadIdle(rtems_task_argument argument)
{
  enterRdbg();
  rtems_task_delete( RTEMS_SELF );
}

/* -----------------------------------------------------------------------
   TgtAttach - attach to a process that is running without control.

   Notes:
      - this function performs a ptrace ATTACH equivalent (attaching to a
	process that we do not control now).
   ----------------------------------------------------------------------- */

Boolean TgtAttach(
  int		conn_idx,	/* client that is requesting */
  PID		pid)		/* process to attach to */
{
  rtems_name task_name;
  rtems_status_code status;
  rtems_id debugId;
  interrupt_gate_descriptor	 *currentIdtEntry;
  unsigned			 limit;
  unsigned			 level;
  
  errno = 0;

  DPRINTF (("TgtAttach pid=%d\n",pid));
  
  Ptrace(RPT_ATTACH, pid, NULL, 0, NULL);
  if (errno)
    return(False);		/* failed */

  TgtCreateNew(pid, conn_idx, 0, NULL, False);


  /*
   *  Connect the Exception used to debug
   */
  i386_get_info_from_IDTR (&currentIdtEntry, &limit);
  
  _CPU_ISR_Disable(level);
  create_interrupt_gate_descriptor (&currentIdtEntry[50], rtems_exception_prologue_50);
  _CPU_ISR_Enable(level);

  old_currentExcHandler = _currentExcHandler;
  _currentExcHandler = BreakPointExcHdl ;


  /*
   * Create the attach debuger task
   */
  task_name = rtems_build_name( 'E', 'v', 'n', 't' );
  if ((status = rtems_task_create( task_name, 10, 24576,
				   RTEMS_INTERRUPT_LEVEL(0),
				   RTEMS_DEFAULT_ATTRIBUTES | RTEMS_SYSTEM_TASK,
				   &eventTaskId ))
      != RTEMS_SUCCESSFUL){
    printf("status = %d\n",status);
    rtems_panic ("Can't create task.\n");
  }
  
  status = rtems_task_start(eventTaskId, eventTask, pid);

  status = rtems_semaphore_create (rtems_build_name('D', 'B', 'G', 's'),
				   1,
				   RTEMS_FIFO |
				   RTEMS_COUNTING_SEMAPHORE |
				   RTEMS_NO_INHERIT_PRIORITY |
				   RTEMS_NO_PRIORITY_CEILING |
				   RTEMS_LOCAL,
				   0,
				   &serializeSemId);
  if (status != RTEMS_SUCCESSFUL)
    rtems_panic ("Can't create serialize semaphore: `%s'\n",rtems_status_text(status));

  status = rtems_semaphore_create (rtems_build_name('D', 'B', 'G', 'w'),
				   0,
				   RTEMS_FIFO |
				   RTEMS_COUNTING_SEMAPHORE |
				   RTEMS_NO_INHERIT_PRIORITY |
				   RTEMS_NO_PRIORITY_CEILING |
				   RTEMS_LOCAL,
				   0,
				   &wakeupEventSemId);
  if (status != RTEMS_SUCCESSFUL)
    rtems_panic ("Can't create wakeup semaphore: `%s'\n",rtems_status_text(status));

  /*
   * Create the MyThreadIdle task to init Exception mechanism 
   */
  task_name = rtems_build_name( 'R', 'i', 'n', 'i' );
  if ((status = rtems_task_create( task_name, 10, 24576,
				   RTEMS_INTERRUPT_LEVEL(0),
				   RTEMS_DEFAULT_ATTRIBUTES,
				   &debugId ))
      != RTEMS_SUCCESSFUL){
    printf("status = %d\n",status);
    rtems_panic ("Can't create task.\n");
  }

  status = rtems_task_start(debugId, MyThreadIdle, pid);

  return(True);
}

/* -----------------------------------------------------------------------
   TgtPtrace - handle ptrace requests for server.
   ----------------------------------------------------------------------- */

int TgtPtrace(		
  int		req,
  PID		pid,
  char		*addr,
  int		data,
  void		*addr2)
{
  if ((req == RPT_SINGLESTEP || req == RPT_CONT)
  &&  addr2)			/* clear then step */
  {				/* addr2 is the old value */
    int		ret;

    errno = 0;
    TgtBreakRestoreOrig (pid, addr, addr2);
    ret = Ptrace(RPT_SINGLESTEP, pid, addr, data, NULL); /* step over */
    if (ret)		/* error, cannot single-step */
    {
      int pid_idx = FindPidEntry (pid);
      TgtBreakCancelStep (&pid_list [pid_idx]);
    }
    return(ret);		/* failed or done */
  }
  else
    return(Ptrace(req, pid, addr, data, addr2)); /* normal call */
}

/* -----------------------------------------------------------------
   TgtGetThreadName - get thread name
   --------------------------------------------------------------- */

int TgtGetThreadName (
   PID_LIST     *plst,		/* Process entry */
   unsigned     Id,             /* Thread ID */
   char    	*ThrName)		/* Thread name */
{
    int index;
    unsigned name;
    
    if ( Id <_Objects_Information_table[OBJECTS_RTEMS_TASKS]->maximum_id &&
	 Id >_Objects_Information_table[OBJECTS_RTEMS_TASKS]->minimum_id) {

      index = Id - _Objects_Information_table[OBJECTS_RTEMS_TASKS]->minimum_id;
      name = *(unsigned*)(_Objects_Information_table[OBJECTS_RTEMS_TASKS]->local_table[1+index]->name);
      ThrName[0] = (char)((name >> 24) & 0xFF );
      ThrName[1] = (char)((name >> 16) & 0xFF );
      ThrName[2] = (char)((name >> 8)  & 0xFF );
      ThrName[3] = (char)( name        & 0xFF );
      ThrName[4] = 0x0;
      return 0;
    }
      
    if ( Id <_Objects_Information_table[OBJECTS_POSIX_THREADS]->maximum_id &&
	 Id >_Objects_Information_table[OBJECTS_POSIX_THREADS]->minimum_id) {

      index = Id - _Objects_Information_table[OBJECTS_POSIX_THREADS]->minimum_id;
      name = *(unsigned*)(_Objects_Information_table[OBJECTS_POSIX_THREADS]->local_table[1+index]->name);
      ThrName[0] = (char)((name >> 24) & 0xFF );
      ThrName[1] = (char)((name >> 16) & 0xFF );
      ThrName[2] = (char)((name >> 8)  & 0xFF );
      ThrName[3] = (char)( name        & 0xFF );
      ThrName[4] = 0x0;
      return 0;
    }
      
	
    return -1;

}

/* -----------------------------------------------------------------
   TgtThreadList - return all the threads in the system
   ----------------------------------------------------------------- */

    int
TgtThreadList (
  PID_LIST* plst,		/* Process entry */
  unsigned* threads,		/* Output buffer */
  unsigned size)		/* Output buffer size */
{
    int curr = 0;
    Objects_Id  id;     
    unsigned index;

    id = _Objects_Information_table[OBJECTS_RTEMS_TASKS]->minimum_id;

    while (id < _Objects_Information_table[OBJECTS_RTEMS_TASKS]->maximum_id){
      index = id - _Objects_Information_table[OBJECTS_RTEMS_TASKS]->minimum_id;
      if ( _Objects_Information_table[OBJECTS_RTEMS_TASKS]->local_table[1+index] != NULL){
      threads[curr] = (unsigned) id;
      curr++;
      }
      id ++;
    }
	
    id = _Objects_Information_table[OBJECTS_POSIX_THREADS]->minimum_id;

    while (id < _Objects_Information_table[OBJECTS_POSIX_THREADS]->maximum_id){
      index = id - _Objects_Information_table[OBJECTS_POSIX_THREADS]->minimum_id;
      if ( _Objects_Information_table[OBJECTS_POSIX_THREADS]->local_table[1+index] != NULL){
      threads[curr] = (unsigned) id;
      curr++;
      }
      id ++;
    }
	
    return curr;
}
