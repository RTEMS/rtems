/*
 *  UNIX Simulator Dependent Source
 *
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Division Incorporated not be
 *      used in advertising or publicity pertaining to distribution
 *      of the software without specific, written prior permission.
 *      Division Incorporated makes no representations about the
 *      suitability of this software for any purpose.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/interr.h>

#if defined(solaris2)
/*
#undef  _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 3
#undef  __STRICT_ANSI__
#define __STRICT_ANSI__
*/
#define __EXTENSIONS__
#endif
 
#if defined(linux)
#define MALLOC_0_RETURNS_NULL
#endif
 
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#ifndef SA_RESTART
#define SA_RESTART 0
#endif

typedef struct {
  jmp_buf   regs;
  sigset_t  isr_level;
} Context_Control_overlay;

void  _CPU_Signal_initialize(void);
void  _CPU_Stray_signal(int);
void  _CPU_ISR_Handler(int);

sigset_t         _CPU_Signal_mask;
Context_Control  _CPU_Context_Default_with_ISRs_enabled;
Context_Control  _CPU_Context_Default_with_ISRs_disabled;

/*
 * Which cpu are we? Used by libcpu and libbsp.
 */

int cpu_number;

/*PAGE
 *
 *  _CPU_ISR_From_CPU_Init
 */

sigset_t  posix_empty_mask;

void _CPU_ISR_From_CPU_Init()
{
  unsigned32        i;
  proc_ptr          old_handler;

  /*
   * Generate an empty mask to be used by disable_support
   */

  sigemptyset(&posix_empty_mask);
 
  /*
   * Block all the signals except SIGTRAP for the debugger
   * and SIGABRT for fatal errors.
   */

  (void) sigfillset(&_CPU_Signal_mask);
  (void) sigdelset(&_CPU_Signal_mask, SIGTRAP);
  (void) sigdelset(&_CPU_Signal_mask, SIGABRT);
  (void) sigdelset(&_CPU_Signal_mask, SIGIOT);
  (void) sigdelset(&_CPU_Signal_mask, SIGCONT);

  _CPU_ISR_Enable(1);

  /*
   * Set the handler for all signals to be signal_handler
   * which will then vector out to the correct handler
   * for whichever signal actually happened. Initially
   * set the vectors to the stray signal handler.
   */

  for (i = 0; i < CPU_INTERRUPT_NUMBER_OF_VECTORS; i++)
      (void)_CPU_ISR_install_vector(i, _CPU_Stray_signal, &old_handler);

  _CPU_Signal_initialize();
}

void _CPU_Signal_initialize( void )
{
  struct sigaction  act;
  sigset_t          mask;

  /* mark them all active except for TraceTrap  and Abort */

  sigfillset(&mask);
  sigdelset(&mask, SIGTRAP);
  sigdelset(&mask, SIGABRT);
  sigdelset(&mask, SIGIOT);
  sigdelset(&mask, SIGCONT);
  sigprocmask(SIG_UNBLOCK, &mask, 0);

  act.sa_handler = _CPU_ISR_Handler;
  act.sa_mask = mask;
  act.sa_flags = SA_RESTART;

  sigaction(SIGHUP, &act, 0);
  sigaction(SIGINT, &act, 0);
  sigaction(SIGQUIT, &act, 0);
  sigaction(SIGILL, &act, 0);
#ifdef SIGEMT
  sigaction(SIGEMT, &act, 0);
#endif
  sigaction(SIGFPE, &act, 0);
  sigaction(SIGKILL, &act, 0);
  sigaction(SIGBUS, &act, 0);
  sigaction(SIGSEGV, &act, 0);
#ifdef SIGSYS
  sigaction(SIGSYS, &act, 0);
#endif
  sigaction(SIGPIPE, &act, 0);
  sigaction(SIGALRM, &act, 0);
  sigaction(SIGTERM, &act, 0);
  sigaction(SIGUSR1, &act, 0);
  sigaction(SIGUSR2, &act, 0);
  sigaction(SIGCHLD, &act, 0);
  sigaction(SIGCLD, &act, 0);
  sigaction(SIGPWR, &act, 0);
  sigaction(SIGVTALRM, &act, 0);
  sigaction(SIGPROF, &act, 0);
  sigaction(SIGIO, &act, 0);
  sigaction(SIGWINCH, &act, 0);
  sigaction(SIGSTOP, &act, 0);
  sigaction(SIGTTIN, &act, 0);
  sigaction(SIGTTOU, &act, 0);
  sigaction(SIGURG, &act, 0);
#ifdef SIGLOST
    sigaction(SIGLOST, &act, 0);
#endif

}

/*PAGE
 *
 *  _CPU_Context_From_CPU_Init
 */

void _CPU_Context_From_CPU_Init()
{

#if defined(hppa1_1) && defined(RTEMS_UNIXLIB_SETJMP)
    /*
     * HACK - set the _SYSTEM_ID to 0x20c so that setjmp/longjmp
     * will handle the full 32 floating point registers.
     * 
     *  NOTE:  Is this a bug in HPUX9?
     */

    {
      extern unsigned32 _SYSTEM_ID;

      _SYSTEM_ID = 0x20c;
    }
#endif

  /*
   *  get default values to use in _CPU_Context_Initialize()
   */

  _CPU_ISR_Set_level( 0 );
  _CPU_Context_switch(
    &_CPU_Context_Default_with_ISRs_enabled,
    &_CPU_Context_Default_with_ISRs_enabled
  );

  _CPU_ISR_Set_level( 1 );
  _CPU_Context_switch(
    &_CPU_Context_Default_with_ISRs_disabled,
    &_CPU_Context_Default_with_ISRs_disabled
  );
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */

sigset_t GET_old_mask;

unsigned32 _CPU_ISR_Get_level( void )
{
/*  sigset_t  old_mask; */
   unsigned32 old_level;
 
  sigprocmask(0, 0, &GET_old_mask);
 
  if (memcmp((void *)&posix_empty_mask, (void *)&GET_old_mask, sizeof(sigset_t)))
    old_level = 1;
  else 
    old_level = 0;

  return old_level;
}

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 */


void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void            (*thread_dispatch)      /* ignored on this CPU */
)
{
  /*
   *  The thread_dispatch argument is the address of the entry point
   *  for the routine called at the end of an ISR once it has been
   *  decided a context switch is necessary.  On some compilation
   *  systems it is difficult to call a high-level language routine
   *  from assembly.  This allows us to trick these systems.
   *
   *  If you encounter this problem save the entry point in a CPU
   *  dependent variable.
   */

  _CPU_Thread_dispatch_pointer = thread_dispatch;

  /*
   * XXX; If there is not an easy way to initialize the FP context
   *      during Context_Initialize, then it is usually easier to
   *      save an "uninitialized" FP context here and copy it to
   *      the task's during Context_Initialize.
   */

  /* XXX: FP context initialization support */

  _CPU_Table = *cpu_table;

  _CPU_ISR_From_CPU_Init();

  _CPU_Context_From_CPU_Init();

}

/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 */

void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  _CPU_Fatal_halt( 0xdeaddead );
}

/*PAGE
 *
 *  _CPU_ISR_install_vector
 *
 *  This kernel routine installs the RTEMS handler for the
 *  specified vector.
 *
 *  Input parameters:
 *    vector      - interrupt vector number
 *    old_handler - former ISR for this vector number
 *    new_handler - replacement ISR for this vector number
 *
 *  Output parameters:  NONE
 *
 */


void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
   *old_handler = _ISR_Vector_table[ vector ];

   /*
    *  If the interrupt vector table is a table of pointer to isr entry
    *  points, then we need to install the appropriate RTEMS interrupt
    *  handler for this vector number.
    */

   /*
    *  We put the actual user ISR address in '_ISR_vector_table'.  This will
    *  be used by the _CPU_ISR_Handler so the user gets control.
    */

    _ISR_Vector_table[ vector ] = new_handler;
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
}

/*PAGE
 *
 *  _CPU_Internal_threads_Idle_thread_body
 *
 *  NOTES:
 *
 *  1. This is the same as the regular CPU independent algorithm.
 *
 *  2. If you implement this using a "halt", "idle", or "shutdown"
 *     instruction, then don't forget to put it in an infinite loop.
 *
 *  3. Be warned. Some processors with onboard DMA have been known
 *     to stop the DMA if the CPU were put in IDLE mode.  This might
 *     also be a problem with other on-chip peripherals.  So use this
 *     hook with caution.
 */

void _CPU_Internal_threads_Idle_thread_body( void )
{
  while (1)
    pause();
}

/*PAGE
 * 
 *  _CPU_Context_Initialize
 */

void _CPU_Context_Initialize(
  Context_Control  *_the_context,
  unsigned32       *_stack_base,
  unsigned32        _size,
  unsigned32        _new_level,
  void             *_entry_point
)
{
  void        *source;
  unsigned32  *addr;
  unsigned32   jmp_addr;
  unsigned32   _stack_low;   /* lowest "stack aligned" address */
  unsigned32   _stack_high;  /* highest "stack aligned" address */
  unsigned32   _the_size;

  jmp_addr = (unsigned32) _entry_point;

  /*
   *  On CPUs with stacks which grow down, we build the stack
   *  based on the _stack_high address.  On CPUs with stacks which 
   *  grow up, we build the stack based on the _stack_low address.  
   */

  _stack_low = ((unsigned32)(_stack_base) + CPU_STACK_ALIGNMENT);
  _stack_low &= ~(CPU_STACK_ALIGNMENT - 1);

  _stack_high = ((unsigned32)(_stack_base) + _size);
  _stack_high &= ~(CPU_STACK_ALIGNMENT - 1);

  _the_size = _size & ~(CPU_STACK_ALIGNMENT - 1);

  /*
   * Slam our jmp_buf template into the context we are creating
   */

  if ( _new_level == 0 )
    source = &_CPU_Context_Default_with_ISRs_enabled;
  else
    source = &_CPU_Context_Default_with_ISRs_disabled;
      
  memcpy(_the_context, source, sizeof(Context_Control) ); /* sizeof(jmp_buf)); */

  addr = (unsigned32 *)_the_context;

#if defined(hppa1_1)
  *(addr + RP_OFF) = jmp_addr;
  *(addr + SP_OFF) = (unsigned32)(_stack_low + CPU_FRAME_SIZE);

  /*
   * See if we are using shared libraries by checking
   * bit 30 in 24 off of newp. If bit 30 is set then
   * we are using shared libraries and the jump address
   * is at what 24 off of newp points to so shove that
   * into 24 off of newp instead.
   */

  if (jmp_addr & 0x40000000) {
    jmp_addr &= 0xfffffffc;
     *(addr + RP_OFF) = (unsigned32)*(unsigned32 *)jmp_addr;
  }
#elif defined(sparc)

  /*
   *  See /usr/include/sys/stack.h in Solaris 2.3 for a nice
   *  diagram of the stack.
   */

  asm ("ta  0x03");            /* flush registers */

  *(addr + RP_OFF) = jmp_addr + ADDR_ADJ_OFFSET;
  *(addr + SP_OFF) = (unsigned32)(_stack_high - CPU_FRAME_SIZE);
  *(addr + FP_OFF) = (unsigned32)(_stack_high);

#elif defined(i386)
 
    /*
     *  This information was gathered by disassembling setjmp().
     */

    {
      unsigned32 stack_ptr;

      stack_ptr = _stack_high - CPU_FRAME_SIZE;

      *(addr + EBX_OFF) = 0xFEEDFEED;
      *(addr + ESI_OFF) = 0xDEADDEAD;
      *(addr + EDI_OFF) = 0xDEAFDEAF;
      *(addr + EBP_OFF) = stack_ptr;
      *(addr + ESP_OFF) = stack_ptr;
      *(addr + RET_OFF) = jmp_addr;
 
      addr = (unsigned32 *) stack_ptr;
 
      addr[ 0 ] = jmp_addr;
      addr[ 1 ] = (unsigned32) stack_ptr;
      addr[ 2 ] = (unsigned32) stack_ptr;
    }

#else
#error "UNKNOWN CPU!!!"
#endif

}

/*PAGE
 *
 *  _CPU_Context_restore
 */

void _CPU_Context_restore(
  Context_Control  *next
)
{
  Context_Control_overlay *nextp = (Context_Control_overlay *)next;

  sigprocmask( SIG_SETMASK, &nextp->isr_level, 0 );
  longjmp( nextp->regs, 0 );
}

/*PAGE
 *
 *  _CPU_Context_switch
 */

void _CPU_Context_switch(
  Context_Control  *current,
  Context_Control  *next
)
{
  Context_Control_overlay *currentp = (Context_Control_overlay *)current;
  Context_Control_overlay *nextp = (Context_Control_overlay *)next;

  int status;

  /*
   *  Switch levels in one operation
   */

  status = sigprocmask( SIG_SETMASK, &nextp->isr_level, &currentp->isr_level );
  if ( status )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      status
    );

  if (setjmp(currentp->regs) == 0) {    /* Save the current context */
     longjmp(nextp->regs, 0);           /* Switch to the new context */
     if ( status )
       _Internal_error_Occurred(
         INTERNAL_ERROR_CORE,
         TRUE,
         status
       );
  }

}
 
/*PAGE
 *
 *  _CPU_Save_float_context
 */

void _CPU_Save_float_context(
  Context_Control_fp *fp_context
)
{
}

/*PAGE
 *
 *  _CPU_Restore_float_context
 */

void _CPU_Restore_float_context(
  Context_Control_fp *fp_context
)
{
}

/*PAGE
 *
 *  _CPU_ISR_Disable_support
 */

unsigned32 _CPU_ISR_Disable_support(void)
{
  int status;
  sigset_t  old_mask;

  status = sigprocmask(SIG_BLOCK, &_CPU_Signal_mask, &old_mask);
  if ( status )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      status
    );

  if (memcmp((void *)&posix_empty_mask, (void *)&old_mask, sizeof(sigset_t)))
    return 1;

  return 0;
}

/*PAGE
 *
 *  _CPU_ISR_Enable
 */

void _CPU_ISR_Enable(
  unsigned32 level
)
{
  int status;

  if (level == 0)
    status = sigprocmask(SIG_UNBLOCK, &_CPU_Signal_mask, 0);
  else
    status = sigprocmask(SIG_BLOCK, &_CPU_Signal_mask, 0);

  if ( status )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      status
    );
}

/*PAGE
 *
 *  _CPU_ISR_Handler
 *
 *  External interrupt handler.
 *  This is installed as a UNIX signal handler.
 *  It vectors out to specific user interrupt handlers.
 */

void _CPU_ISR_Handler(int vector)
{
  extern void        _Thread_Dispatch(void);
  extern unsigned32  _Thread_Dispatch_disable_level;
  extern boolean     _Context_Switch_necessary;


  if (_ISR_Nest_level++ == 0) {
      /* switch to interrupt stack */
  }

  _Thread_Dispatch_disable_level++;

  if (_ISR_Vector_table[vector]) {
     _ISR_Vector_table[vector](vector);
  } else {
     _CPU_Stray_signal(vector);
  }

  if (_ISR_Nest_level-- == 0) {
      /* switch back to original stack */
  }

  _Thread_Dispatch_disable_level--;

  if (_Thread_Dispatch_disable_level == 0 &&
      (_Context_Switch_necessary || _ISR_Signals_to_thread_executing)) {
      _CPU_ISR_Enable(0);
      _Thread_Dispatch();
  }
}

/*PAGE
 *
 *  _CPU_Stray_signal
 */

void _CPU_Stray_signal(int sig_num)
{
  char buffer[ 4 ];   

  /* 
   *  We avoid using the stdio section of the library.
   *  The following is generally safe.
   */

  buffer[ 0 ] = (sig_num >> 4) + 0x30;
  buffer[ 1 ] = (sig_num & 0xf) + 0x30;
  buffer[ 2 ] = '\n';

  write( 2, "Stray signal 0x", 12 );
  write( 2, buffer, 3 );
 
  /*
   * If it was a "fatal" signal, then exit here
   * If app code has installed a hander for one of these, then
   * we won't call _CPU_Stray_signal, so this is ok.
   */
 
  switch (sig_num) {
      case SIGINT:
      case SIGHUP:
      case SIGQUIT:
      case SIGILL:
#ifdef SIGEMT
      case SIGEMT:
#endif
      case SIGKILL:
      case SIGBUS:
      case SIGSEGV:
      case SIGTERM:
          _CPU_Fatal_error(0x100 + sig_num);
  }
}

/*PAGE
 *
 *  _CPU_Fatal_error
 */

void _CPU_Fatal_error(unsigned32 error)
{
  setitimer(ITIMER_REAL, 0, 0);

  if ( error ) {
#ifdef RTEMS_DEBUG
    abort();
#endif
    if (getenv("RTEMS_DEBUG"))
      abort();
  }

  _exit(error);
}

/*PAGE
 *
 *  _CPU_ffs
 */

int _CPU_ffs(unsigned32 value)
{
  int output;
  extern int ffs( int );

  output = ffs(value);
  output = output - 1;

  return output;
}


/*
 *  Special Purpose Routines to hide the use of UNIX system calls.
 */

#if 0
/* XXX clock had this set of #define's */

/*
 *  In order to get the types and prototypes used in this file under
 *  Solaris 2.3, it is necessary to pull the following magic.
 */
 
#if defined(solaris)
#warning "Ignore the undefining __STDC__ warning"
#undef __STDC__
#define __STDC__ 0
#undef  _POSIX_C_SOURCE
#endif
#endif

int _CPU_Get_clock_vector( void )
{
  return SIGALRM;
}


void _CPU_Start_clock( 
  int microseconds
)
{
  struct itimerval  new;

  new.it_value.tv_sec = 0;
  new.it_value.tv_usec = microseconds;
  new.it_interval.tv_sec = 0;
  new.it_interval.tv_usec = microseconds;

  setitimer(ITIMER_REAL, &new, 0);
}

void _CPU_Stop_clock( void )
{
  struct itimerval  new;
  struct sigaction  act;
 
  /*
   * Set the SIGALRM signal to ignore any last
   * signals that might come in while we are
   * disarming the timer and removing the interrupt
   * vector.
   */
 
  act.sa_handler = SIG_IGN;

  sigaction(SIGALRM, &act, 0);
 
  new.it_value.tv_sec = 0;
  new.it_value.tv_usec = 0;
 
  setitimer(ITIMER_REAL, &new, 0);
}

int  _CPU_SHM_Semid;
extern       void fix_syscall_errno( void );

void _CPU_SHM_Init( 
  unsigned32   maximum_nodes,
  boolean      is_master_node,
  void       **shm_address,
  unsigned32  *shm_length
)
{
  int          i;
  int          shmid;
  char        *shm_addr;
  key_t        shm_key;
  key_t        sem_key;
  int          status;
  int          shm_size;
 
  if (getenv("RTEMS_SHM_KEY"))
    shm_key = strtol(getenv("RTEMS_SHM_KEY"), 0, 0);
  else
#ifdef RTEMS_SHM_KEY
    shm_key = RTEMS_SHM_KEY;
#else
    shm_key = 0xa000;
#endif
 
    if (getenv("RTEMS_SHM_SIZE"))
      shm_size = strtol(getenv("RTEMS_SHM_SIZE"), 0, 0);
    else
#ifdef RTEMS_SHM_SIZE
      shm_size = RTEMS_SHM_SIZE;
#else
      shm_size = 64 * 1024;
#endif
 
    if (getenv("RTEMS_SHM_SEMAPHORE_KEY"))
      sem_key = strtol(getenv("RTEMS_SHM_SEMAPHORE_KEY"), 0, 0);
    else
#ifdef RTEMS_SHM_SEMAPHORE_KEY
      sem_key = RTEMS_SHM_SEMAPHORE_KEY;
#else
      sem_key = 0xa001;
#endif
 
    shmid = shmget(shm_key, shm_size, IPC_CREAT | 0660);
    if ( shmid == -1 ) {
      fix_syscall_errno(); /* in case of newlib */
      perror( "shmget" );
      _CPU_Fatal_halt( 0xdead0001 );
    }
 
    shm_addr = shmat(shmid, (char *)0, SHM_RND);
    if ( shm_addr == (void *)-1 ) {
      fix_syscall_errno(); /* in case of newlib */
      perror( "shmat" );
      _CPU_Fatal_halt( 0xdead0002 );
    }
 
    _CPU_SHM_Semid = semget(sem_key, maximum_nodes + 1, IPC_CREAT | 0660);
    if ( _CPU_SHM_Semid == -1 ) {
      fix_syscall_errno(); /* in case of newlib */
      perror( "semget" );
      _CPU_Fatal_halt( 0xdead0003 );
    }
 
    if ( is_master_node ) {
      for ( i=0 ; i <= maximum_nodes ; i++ ) {
#if defined(solaris2)
        union semun {
          int val;
          struct semid_ds *buf;
          ushort *array;
        } help;
 
        help.val = 1;
        status = semctl( _CPU_SHM_Semid, i, SETVAL, help );
#endif
#if defined(hpux)
        status = semctl( _CPU_SHM_Semid, i, SETVAL, 1 );
#endif
 
        fix_syscall_errno(); /* in case of newlib */
        if ( status == -1 ) {
          _CPU_Fatal_halt( 0xdead0004 );
        }
      }
    }
 
  *shm_address = shm_addr;
  *shm_length = shm_size;

}

int _CPU_Get_pid( void )
{
  return getpid();
}

/*
 * Define this to use signals for MPCI shared memory driver.
 * If undefined, the shared memory driver will poll from the
 * clock interrupt.
 * Ref: ../shmsupp/getcfg.c
 *
 * BEWARE:: many UN*X kernels and debuggers become severely confused when
 *          debugging programs which use signals.  The problem is *much*
 *          worse when using multiple signals, since ptrace(2) tends to
 *          drop all signals except 1 in the case of multiples.
 *          On hpux9, this problem was so bad, we couldn't use interrupts
 *          with the shared memory driver if we ever hoped to debug
 *          RTEMS programs.
 *          Maybe systems that use /proc don't have this problem...
 */
 
 
int _CPU_SHM_Get_vector( void )
{
#ifdef CPU_USE_SHM_INTERRUPTS
  return SIGUSR1;
#else
  return 0;
#endif
}

void _CPU_SHM_Send_interrupt(
  int pid,
  int vector
)
{
  kill((pid_t) pid, vector);
}

void _CPU_SHM_Lock( 
  int semaphore
)
{
  struct sembuf      sb;
  int                status;
 
  sb.sem_num = semaphore;
  sb.sem_op  = -1;
  sb.sem_flg = 0;
 
  while (1) {
    status = semop(_CPU_SHM_Semid, &sb, 1);
    if ( status >= 0 )
      break;
    if ( status == -1 ) {
       fix_syscall_errno();    /* in case of newlib */
        if (errno == EINTR)
            continue;
        perror("shm lock");
        _CPU_Fatal_halt( 0xdead0005 );
    }
  }

}

void _CPU_SHM_Unlock(
  int semaphore
)
{
  struct sembuf  sb;
  int            status;
 
  sb.sem_num = semaphore;
  sb.sem_op  = 1;
  sb.sem_flg = 0;
 
  while (1) {
    status = semop(_CPU_SHM_Semid, &sb, 1);
    if ( status >= 0 )
      break;
 
    if ( status == -1 ) {
      fix_syscall_errno();    /* in case of newlib */
      if (errno == EINTR)
          continue;
      perror("shm unlock");
      _CPU_Fatal_halt( 0xdead0006 );
    }
  }

}
