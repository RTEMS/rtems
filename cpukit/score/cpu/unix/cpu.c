/*
 *  UNIX Simulator Dependent Source
 *
 *  COPYRIGHT (c) 1994,95 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/interr.h>

#if defined(__linux__)
#define _XOPEN_SOURCE
#define MALLOC_0_RETURNS_NULL
#endif

#include <sys/types.h>
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#endif
#include <string.h>   /* memset */

#ifndef SA_RESTART
#define SA_RESTART 0
#endif

typedef struct {
  jmp_buf   regs;
  int       isr_level;
} Context_Control_overlay;

void  _CPU_Signal_initialize(void);
void  _CPU_Stray_signal(int);
void  _CPU_ISR_Handler(int);

static sigset_t _CPU_Signal_mask;
static Context_Control_overlay _CPU_Context_Default_with_ISRs_enabled;
static Context_Control_overlay _CPU_Context_Default_with_ISRs_disabled;

/*
 * Sync IO support, an entry for each fd that can be set
 */

void  _CPU_Sync_io_Init();

static rtems_sync_io_handler _CPU_Sync_io_handlers[FD_SETSIZE];
static int sync_io_nfds;
static fd_set sync_io_readfds;
static fd_set sync_io_writefds;
static fd_set sync_io_exceptfds;

/*
 * Which cpu are we? Used by libcpu and libbsp.
 */

int cpu_number;

/*PAGE
 *
 *  _CPU_Initialize_vectors()
 *
 *  Support routine to initialize the RTEMS vector table after it is allocated.
 *
 *  UNIX Specific Information:
 *
 *  Complete initialization since the table is now allocated.
 */
 
sigset_t  posix_empty_mask;

void _CPU_Initialize_vectors(void)
{
  unsigned32        i;
  proc_ptr          old_handler;

  /*
   * Generate an empty mask to be used by disable_support
   */

  sigemptyset(&posix_empty_mask);

  /*
   * Block all the signals except SIGTRAP for the debugger
   * and fatal error signals.
   */

  (void) sigfillset(&_CPU_Signal_mask);
  (void) sigdelset(&_CPU_Signal_mask, SIGTRAP);
  (void) sigdelset(&_CPU_Signal_mask, SIGABRT);
#if !defined(__CYGWIN__)
  (void) sigdelset(&_CPU_Signal_mask, SIGIOT);
#endif
  (void) sigdelset(&_CPU_Signal_mask, SIGCONT);
  (void) sigdelset(&_CPU_Signal_mask, SIGSEGV);
  (void) sigdelset(&_CPU_Signal_mask, SIGBUS);
  (void) sigdelset(&_CPU_Signal_mask, SIGFPE);

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

  mask = _CPU_Signal_mask;
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
#ifdef SIGCLD
  sigaction(SIGCLD, &act, 0);
#endif
#ifdef SIGPWR
  sigaction(SIGPWR, &act, 0);
#endif
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

#if defined(__hppa__) && defined(RTEMS_UNIXLIB_SETJMP)
    /*
     * HACK - set the _SYSTEM_ID to 0x20c so that setjmp/longjmp
     * will handle the full 32 floating point registers.
     */

    {
      extern unsigned32 _SYSTEM_ID;

      _SYSTEM_ID = 0x20c;
    }
#endif

  /*
   *  get default values to use in _CPU_Context_Initialize()
   */

  if ( sizeof(Context_Control_overlay) > sizeof(Context_Control) )
    _CPU_Fatal_halt( 0xdeadf00d );
  
  (void) memset(
    &_CPU_Context_Default_with_ISRs_enabled,
    0,
    sizeof(Context_Control_overlay)
  );
  (void) memset(
    &_CPU_Context_Default_with_ISRs_disabled,
    0,
    sizeof(Context_Control_overlay)
  );

  _CPU_ISR_Set_level( 0 );
  _CPU_Context_switch(
    (Context_Control *) &_CPU_Context_Default_with_ISRs_enabled,
    (Context_Control *) &_CPU_Context_Default_with_ISRs_enabled
  );

  _CPU_ISR_Set_level( 1 );
  _CPU_Context_switch(
    (Context_Control *) &_CPU_Context_Default_with_ISRs_disabled,
    (Context_Control *) &_CPU_Context_Default_with_ISRs_disabled
  );
}

/*PAGE
 *
 *  _CPU_Sync_io_Init
 */

void _CPU_Sync_io_Init()
{
  int fd;

  for (fd = 0; fd < FD_SETSIZE; fd++)
    _CPU_Sync_io_handlers[fd] = NULL;

  sync_io_nfds = 0;
  FD_ZERO(&sync_io_readfds);
  FD_ZERO(&sync_io_writefds);
  FD_ZERO(&sync_io_exceptfds);
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */

unsigned32 _CPU_ISR_Get_level( void )
{
  sigset_t old_mask;

  sigemptyset( &old_mask );
  sigprocmask(SIG_BLOCK, 0, &old_mask);

  if (memcmp((void *)&posix_empty_mask, (void *)&old_mask, sizeof(sigset_t)))
      return 1;

  return 0;
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
   *  If something happened where the public Context_Control is not
   *  at least as large as the private Context_Control_overlay, then
   *  we are in trouble.
   */

  if ( sizeof(Context_Control_overlay) > sizeof(Context_Control) )
    _CPU_Fatal_error(0x100 + 1);

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

  _CPU_Sync_io_Init();

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
 *  _CPU_Thread_Idle_body
 *
 *  Stop until we get a signal which is the logically the same thing
 *  entering low-power or sleep mode on a real processor and waiting for
 *  an interrupt.  This significantly reduces the consumption of host
 *  CPU cycles which is again similar to low power mode.
 */

void _CPU_Thread_Idle_body( void )
{
#if CPU_SYNC_IO
  extern void _Thread_Dispatch(void);
  int fd;
#endif

  while (1) {
#ifdef RTEMS_DEBUG
    /* interrupts had better be enabled at this point! */
    if (_CPU_ISR_Get_level() != 0)
       abort();
#endif

    /*
     *  Block on a select statement, the CPU interface added allow the
     *  user to add new descriptors which are to be blocked on
     */

#if CPU_SYNC_IO
    if (sync_io_nfds) {
      int result;
      fd_set readfds, writefds, exceptfds;

      readfds = sync_io_readfds;
      writefds = sync_io_writefds;
      exceptfds = sync_io_exceptfds;
      result = select(sync_io_nfds,
                 &readfds,
                 &writefds,
                 &exceptfds,
                 NULL);

      if (result < 0) {
	if (errno != EINTR)
	  _CPU_Fatal_error(0x200);       /* FIXME : what number should go here !! */
	_Thread_Dispatch();
	continue;
      }

      for (fd = 0; fd < sync_io_nfds; fd++) {
        boolean read = FD_ISSET(fd, &readfds);
        boolean write = FD_ISSET(fd, &writefds);
        boolean except = FD_ISSET(fd, &exceptfds);

        if (_CPU_Sync_io_handlers[fd] && (read || write || except))
          _CPU_Sync_io_handlers[fd](fd, read, write, except);
      }

      _Thread_Dispatch();
    } else
      pause();
#else
    pause();
#endif

  }

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
  void             *_entry_point,
  boolean           _is_fp
)
{
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

  _stack_low = (unsigned32)(_stack_base) + CPU_STACK_ALIGNMENT - 1;
  _stack_low &= ~(CPU_STACK_ALIGNMENT - 1);

  _stack_high = (unsigned32)(_stack_base) + _size;
  _stack_high &= ~(CPU_STACK_ALIGNMENT - 1);

  if (_stack_high > _stack_low)
    _the_size = _stack_high - _stack_low;
  else
    _the_size = _stack_low - _stack_high;

  /*
   * Slam our jmp_buf template into the context we are creating
   */

  if ( _new_level == 0 )
      *(Context_Control_overlay *)_the_context =
                         _CPU_Context_Default_with_ISRs_enabled;
  else
      *(Context_Control_overlay *)_the_context = 
                         _CPU_Context_Default_with_ISRs_disabled;

  addr = (unsigned32 *)_the_context;

#if defined(__hppa__)
  *(addr + RP_OFF) = jmp_addr;
  *(addr + SP_OFF) = (unsigned32)(_stack_low + CPU_FRAME_SIZE);

  /*
   * See if we are using shared libraries by checking
   * bit 30 in 24 off of newp. If bit 30 is set then
   * we are using shared libraries and the jump address
   * points to the pointer, so we put that into rp instead.
   */

  if (jmp_addr & 0x40000000) {
    jmp_addr &= 0xfffffffc;
     *(addr + RP_OFF) = *(unsigned32 *)jmp_addr;
  }
#elif defined(__sparc__)

  /*
   *  See /usr/include/sys/stack.h in Solaris 2.3 for a nice
   *  diagram of the stack.
   */

  asm ("ta  0x03");            /* flush registers */

  *(addr + RP_OFF) = jmp_addr + ADDR_ADJ_OFFSET;
  *(addr + SP_OFF) = (unsigned32)(_stack_high - CPU_FRAME_SIZE);
  *(addr + FP_OFF) = (unsigned32)(_stack_high);

#elif defined(__i386__)

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

  _CPU_ISR_Enable(nextp->isr_level);
  longjmp( nextp->regs, 0 );
}

/*PAGE
 *
 *  _CPU_Context_switch
 */

static void do_jump(
  Context_Control_overlay *currentp,
  Context_Control_overlay *nextp
);

void _CPU_Context_switch(
  Context_Control  *current,
  Context_Control  *next
)
{
  Context_Control_overlay *currentp = (Context_Control_overlay *)current;
  Context_Control_overlay *nextp = (Context_Control_overlay *)next;
#if 0
  int status;
#endif

  currentp->isr_level = _CPU_ISR_Disable_support();

  do_jump( currentp, nextp );

#if 0
  if (sigsetjmp(currentp->regs, 1) == 0) {    /* Save the current context */
     siglongjmp(nextp->regs, 0);           /* Switch to the new context */
     _Internal_error_Occurred(
         INTERNAL_ERROR_CORE,
         TRUE,
         status
       );
  }
#endif

#ifdef RTEMS_DEBUG
    if (_CPU_ISR_Get_level() == 0)
       abort();
#endif

  _CPU_ISR_Enable(currentp->isr_level);
}

static void do_jump(
  Context_Control_overlay *currentp,
  Context_Control_overlay *nextp
)
{
  int status;

  if (setjmp(currentp->regs) == 0) {    /* Save the current context */
     longjmp(nextp->regs, 0);           /* Switch to the new context */
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

  sigemptyset( &old_mask );
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
      _ISR_Signals_to_thread_executing = FALSE;
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
   * print "stray" msg about ones which that might mean something
   * Avoid using the stdio section of the library.
   * The following is generally safe.
   */

  switch (sig_num)
  {
#ifdef SIGCLD
      case SIGCLD:
          break;
#endif
      default:
      {
        /*
         *  We avoid using the stdio section of the library.
         *  The following is generally safe
         */

        int digit;
        int number = sig_num;
        int len = 0;

        digit = number / 100;
        number %= 100;
        if (digit) buffer[len++] = '0' + digit;

        digit = number / 10;
        number %= 10;
        if (digit || len) buffer[len++] = '0' + digit;

        digit = number;
        buffer[len++] = '0' + digit;

        buffer[ len++ ] = '\n';

        write( 2, "Stray signal ", 13 );
        write( 2, buffer, len );

      }
  }

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
#if !defined(__CYGWIN__)
      case SIGIOT:
#endif
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

/*
 *  Special Purpose Routines to hide the use of UNIX system calls.
 */

int _CPU_Set_sync_io_handler(
  int fd,
  boolean read,
  boolean write,
  boolean except,
  rtems_sync_io_handler handler
)
{
  if ((fd < FD_SETSIZE) && (_CPU_Sync_io_handlers[fd] == NULL)) {
    if (read)
      FD_SET(fd, &sync_io_readfds);
    else
      FD_CLR(fd, &sync_io_readfds);
    if (write)
      FD_SET(fd, &sync_io_writefds);
    else
      FD_CLR(fd, &sync_io_writefds);
    if (except)
      FD_SET(fd, &sync_io_exceptfds);
    else
      FD_CLR(fd, &sync_io_exceptfds);
    _CPU_Sync_io_handlers[fd] = handler;
    if ((fd + 1) > sync_io_nfds)
      sync_io_nfds = fd + 1;
    return 0;
  }
  return -1;
}

int _CPU_Clear_sync_io_handler(
  int fd
)
{
  if ((fd < FD_SETSIZE) && _CPU_Sync_io_handlers[fd]) {
    FD_CLR(fd, &sync_io_readfds);
    FD_CLR(fd, &sync_io_writefds);
    FD_CLR(fd, &sync_io_exceptfds);
    _CPU_Sync_io_handlers[fd] = NULL;
    sync_io_nfds = 0;
    for (fd = 0; fd < FD_SETSIZE; fd++)
      if (FD_ISSET(fd, &sync_io_readfds) ||
          FD_ISSET(fd, &sync_io_writefds) ||
          FD_ISSET(fd, &sync_io_exceptfds))
        sync_io_nfds = fd + 1;
    return 0;
  }
  return -1;
}

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

  (void) memset(&act, 0, sizeof(act));
  act.sa_handler = SIG_IGN;

  sigaction(SIGALRM, &act, 0);

  (void) memset(&new, 0, sizeof(new));
  setitimer(ITIMER_REAL, &new, 0);
}

#if 0
extern void fix_syscall_errno( void );
#endif
#define fix_syscall_errno() 

#if defined(RTEMS_MULTIPROCESSING)
int  _CPU_SHM_Semid;

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
  int          status = 0;  /* to avoid unitialized warnings */
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
#if !HAS_UNION_SEMUN
        union semun {
          int val;
          struct semid_ds *buf;
          unsigned short int *array;
#if defined(__linux__)
          struct seminfo *__buf;
#endif          
        } ;
#endif
        union semun help ;
        help.val = 1;
        status = semctl( _CPU_SHM_Semid, i, SETVAL, help );

        fix_syscall_errno(); /* in case of newlib */
        if ( status == -1 ) {
          _CPU_Fatal_halt( 0xdead0004 );
        }
      }
    }

  *shm_address = shm_addr;
  *shm_length = shm_size;

}
#endif

int _CPU_Get_pid( void )
{
  return getpid();
}

#if defined(RTEMS_MULTIPROCESSING)
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
  struct sembuf sb;

  sb.sem_num = semaphore;
  sb.sem_op  = -1;
  sb.sem_flg = 0;

  while (1) {
    int status = -1;

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
#endif
