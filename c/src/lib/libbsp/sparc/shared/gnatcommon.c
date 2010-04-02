/*
 *
 * Support for gnat/rtems interrupts and exception handling.
 * Jiri Gaisler, ESA/ESTEC, 17-02-1999.
 *
 */

#include <unistd.h> /* getpid */

#include <bsp.h>
#include <rtems/bspIo.h>
#include <signal.h>
#include <stdlib.h>

/*
 * Synchronous trap handler. Map the trap number of SIGFPE, SIGSEGV
 * or SIGILL to generate the corresponding Ada exception.
 */

rtems_isr __gnat_exception_handler
  (rtems_vector_number trap)
{
  uint32_t         real_trap;
  uint32_t         signal;

  real_trap = SPARC_REAL_TRAP_NUMBER (trap);
  switch (real_trap)
    {
    case 0x08:			/* FPU exception */
    case 0x0A:			/* TAG overflow */
    case 0x82:			/* divide by zero */
      signal = SIGFPE;		/* Will cause Constraint_Error */
      break;
    case 0x01:			/* Instruction access exception */
    case 0x09:			/* Data access exception */
      signal = SIGSEGV;		/* Will cause Storage_Error */
      break;
    default:			/* Anything else ... */
      signal = SIGILL;		/* Will cause Program_Error */
      break;
    }
  kill (getpid (), signal);
}

/*
 * Asynchronous trap handler. As it happens, the interrupt trap numbers for
 * SPARC is 17 - 31, so we just map then directly on the same signal number.
 */

rtems_isr __gnat_interrupt_handler
  (rtems_vector_number trap)
{
  uint32_t         real_trap;

  real_trap = SPARC_REAL_TRAP_NUMBER (trap);

  kill (getpid (), real_trap);

}

/*
 * Default signal handler with error reporting
 */

void
__gnat_signals_Abnormal_termination_handler (int signo)
{
  switch (signo)
    {
    case SIGFPE:
      printk("\nConstraint_Error\n");
      break;
    case SIGSEGV:
      printk("\nStorage_Error\n");
      break;
    default:
      printk("\nProgram_Error\n");
      break;
    }
  exit (1);
}

const struct sigaction __gnat_error_vector =
{0, -1,
 {__gnat_signals_Abnormal_termination_handler}};

void
__gnat_install_handler_common (int t1, int t2)
{
  uint32_t         trap;
  rtems_isr_entry previous_isr;

  sigaction (SIGSEGV, &__gnat_error_vector, NULL);
  sigaction (SIGFPE, &__gnat_error_vector, NULL);
  sigaction (SIGILL, &__gnat_error_vector, NULL);

  for (trap = 0; trap < 256; trap++)
    {

      /*
         *  Skip window overflow, underflow, and flush as well as software
         *  trap 0 which we will use as a shutdown. Also avoid trap 0x70 - 0x7f
         *  which cannot happen and where some of the space is used to pass
         *  paramaters to the program.  0x80 for system traps and
	 *  0x81 - 0x83 by the remote debugging stub.
	 *  Avoid two bsp specific interrupts which normally are used
	 *  by the real-time clock and UART B.
       */

      if ((trap >= 0x11) && (trap <= 0x1f))
	{
	  if ((trap != t1) && (trap != t2))
	    rtems_interrupt_catch (__gnat_interrupt_handler, trap, &previous_isr);
	}
      else if ((trap != 5 && trap != 6) && ((trap < 0x70) || (trap > 0x83)))
	set_vector (__gnat_exception_handler, SPARC_SYNCHRONOUS_TRAP (trap), 1);
    }
}
