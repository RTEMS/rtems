/*
 *  HP PA-RISC CPU Dependent Source
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
#include <rtems/fatal.h>
#include <rtems/isr.h>
#include <rtems/wkspace.h>
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

extern void           set_vector(proc_ptr, int, int);
extern void           _Thread_Dispatch(void);

extern unsigned32 _Thread_Dispatch_disable_level;
extern unsigned32 _SYSTEM_ID;
extern boolean    _Context_Switch_necessary;


rtems_status_code signal_initialize(void);
void         Stray_signal(int);
void         signal_enable(unsigned32);
void         signal_disable(unsigned32);
void         interrupt_handler();

sigset_t   UNIX_SIGNAL_MASK;
jmp_buf    default_context;

/*
 * Which cpu are we? Used by libcpu and libbsp.
 */

int cpu_number;

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
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
  unsigned32  i;

  if ( cpu_table == NULL )
    rtems_fatal_error_occurred( RTEMS_NOT_CONFIGURED );

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

#if defined(hppa1_1) && defined(RTEMS_UNIXLIB)
    /*
     * HACK - set the _SYSTEM_ID to 0x20c so that setjmp/longjmp
     * will handle the full 32 floating point registers.
     * 
     *  NOTE:  Is this a bug in HPUX9?
     */

    _SYSTEM_ID = 0x20c;
#endif

  /*
   *  get default values to use in _CPU_Context_Initialize()
   */

   setjmp(default_context);

  /*
   * Block all the signals except SIGTRAP for the debugger
   * and SIGABRT for fatal errors.
   */

  _CPU_ISR_Set_signal_level(1);

  sigfillset(&UNIX_SIGNAL_MASK);
  sigdelset(&UNIX_SIGNAL_MASK, SIGTRAP);
  sigdelset(&UNIX_SIGNAL_MASK, SIGABRT);
  sigdelset(&UNIX_SIGNAL_MASK, SIGIOT);
  sigdelset(&UNIX_SIGNAL_MASK, SIGCONT);

  sigprocmask(SIG_BLOCK, &UNIX_SIGNAL_MASK, 0);

  /*
   * Set the handler for all signals to be signal_handler
   * which will then vector out to the correct handler
   * for whichever signal actually happened. Initially
   * set the vectors to the stray signal handler.
   */

  for (i = 0; i < 32; i++)
      (void)set_vector(Stray_signal, i, 1);

  signal_initialize();
}

/*  _CPU_ISR_install_vector
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
    *  be used by the _ISR_Handler so the user gets control.
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

void _CPU_Context_Initialize(
  Context_Control  *_the_context,
  unsigned32       *_stack_base,
  unsigned32        _size,
  unsigned32        _new_level,
  void             *_entry_point
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

    _stack_low = ((unsigned32)(_stack_base) + CPU_STACK_ALIGNMENT);
    _stack_low &= ~(CPU_STACK_ALIGNMENT - 1);

    _stack_high = ((unsigned32)(_stack_base) + _size);
    _stack_high &= ~(CPU_STACK_ALIGNMENT - 1);

    _the_size = _size & ~(CPU_STACK_ALIGNMENT - 1);

    /*
     * Slam our jmp_buf template into the context we are creating
     */

    memcpy(_the_context, default_context, sizeof(jmp_buf));

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
#else
#error "UNKNOWN CPU!!!"
#endif

    if (_new_level)
        _CPU_ISR_Set_signal_level(1);
    else
        _CPU_ISR_Set_signal_level(0);

}

void _CPU_Context_restore(
  Context_Control  *next
)
{
    longjmp(next->regs, 0);
}

void _CPU_Context_switch(
  Context_Control  *current,
  Context_Control  *next
)
{
    /*
     * Save the current context
     */

    if (setjmp(current->regs) == 0) {

       /*
        * Switch to the new context
        */

       longjmp(next->regs, 0);
    }
}

void _CPU_Save_float_context(
  Context_Control_fp *fp_context
)
{
}

void _CPU_Restore_float_context(
  Context_Control_fp *fp_context
)
{
}

void _CPU_ISR_Set_signal_level(unsigned32 level)
{
    if (level)
        _CPU_Disable_signal();
    else
        _CPU_Enable_signal(0);
}


unsigned32 _CPU_Disable_signal(void)
{
    sigset_t  old_mask;
    sigset_t  empty_mask;

    sigemptyset(&empty_mask);
    sigemptyset(&old_mask);
    sigprocmask(SIG_BLOCK, &UNIX_SIGNAL_MASK, &old_mask);

    if (memcmp((char *)&empty_mask, (char *)&old_mask, sizeof(sigset_t)) != 0)
        return 1;

    return 0;
}


void _CPU_Enable_signal(unsigned32 level)
{
    if (level == 0)
        sigprocmask(SIG_UNBLOCK, &UNIX_SIGNAL_MASK, 0);
}


/*
 * Support for external and spurious interrupts on HPPA
 *
 *  TODO:
 *    delete interrupt.c etc.
 *    Count interrupts
 *    make sure interrupts disabled properly
 *    should handler check again for more interrupts before exit?
 *    How to enable interrupts from an interrupt handler?
 *    Make sure there is an entry for everything in ISR_Vector_Table
 */

/*
 * Init the external interrupt scheme
 * called by bsp_start()
 */

rtems_status_code
signal_initialize(void)
{
    struct sigaction act;
    sigset_t         mask;

    /* mark them all active except for TraceTrap  and Abort */

    sigfillset(&mask);
    sigdelset(&mask, SIGTRAP);
    sigdelset(&mask, SIGABRT);
    sigdelset(&mask, SIGIOT);
    sigdelset(&mask, SIGCONT);
    sigprocmask(SIG_UNBLOCK, &mask, 0);

    act.sa_handler = interrupt_handler;
    act.sa_mask = mask;
#if defined(solaris)
    act.sa_flags = SA_RESTART;
#else
    act.sa_flags = 0;
#endif

    sigaction(SIGHUP, &act, 0);
    sigaction(SIGINT, &act, 0);
    sigaction(SIGQUIT, &act, 0);
    sigaction(SIGILL, &act, 0);
    sigaction(SIGEMT, &act, 0);
    sigaction(SIGFPE, &act, 0);
    sigaction(SIGKILL, &act, 0);
    sigaction(SIGBUS, &act, 0);
    sigaction(SIGSEGV, &act, 0);
    sigaction(SIGSYS, &act, 0);
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
/*
 *  XXX: Really should be on HPUX.
 */

#if defined(hppa1_1)
    sigaction(SIGLOST, &act, 0);
#endif

    return RTEMS_SUCCESSFUL;
}


/*
 * External interrupt handler.
 * This is installed as cpu interrupt handler.
 * It vectors out to specific external interrupt handlers.
 */

void
interrupt_handler(int vector)
{
    if (_ISR_Nest_level++ == 0) {
        /* switch to interrupt stack */
    }

    _Thread_Dispatch_disable_level++;

    if (_ISR_Vector_table[vector]) {
       _ISR_Vector_table[vector](vector);
    }
    else {
       Stray_signal(vector);
    }

    if (_ISR_Nest_level-- == 0) {
        /* switch back to original stack */
    }

    _Thread_Dispatch_disable_level--;

    if (_Thread_Dispatch_disable_level == 0 &&
        (_Context_Switch_necessary || _ISR_Signals_to_thread_executing)) {
        _CPU_Enable_signal(0);
        _Thread_Dispatch();
    }
}


void
Stray_signal(int sig_num)
{
    char buffer[ 80 ];   

    /* 
     *  We avoid using the stdio section of the library.
     *  The following is generally safe.
     */

    write( 
      2,
      buffer, 
      sprintf( buffer, "Stray signal %d\n", sig_num )
    );
 
    /*
     * If it was a "fatal" signal, then exit here
     * If app code has installed a hander for one of these, then
     * we won't call Stray_signal, so this is ok.
     */
 
    switch (sig_num)
    {
        case SIGINT:
        case SIGHUP:
        case SIGQUIT:
        case SIGILL:
        case SIGEMT:
        case SIGKILL:
        case SIGBUS:
        case SIGSEGV:
        case SIGTERM:
            _CPU_Fatal_error(0x100 + sig_num);
    }
}


void
_CPU_Fatal_error(unsigned32 error)
{
    setitimer(ITIMER_REAL, 0, 0);

    _exit(error);
}

int
_CPU_ffs(unsigned32 value)
{
    int output;

    output = ffs(value);
    output = output - 1;

    return(output);
}
