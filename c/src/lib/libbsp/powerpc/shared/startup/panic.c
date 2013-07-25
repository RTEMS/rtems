#include <rtems.h>
#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/uart.h>
#include <rtems/bspIo.h>
#include <rtems/error.h>
#include <libcpu/stackTrace.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threaddispatch.h>

static void
rebootQuestion(void)
{
  printk("Press a key to reboot\n");
  BSP_poll_char_via_serial();
  bsp_reset();
}

void BSP_panic(char *s)
{
  printk("%s PANIC %s\n",_RTEMS_version, s);
  rebootQuestion();
}

#define THESRC _Internal_errors_What_happened.the_source
#define ISITNL _Internal_errors_What_happened.is_internal
#define THEERR _Internal_errors_What_happened.the_error

void _BSP_Fatal_error(unsigned int v)
{
  unsigned long flags;
  const char *err = 0;

  rtems_interrupt_disable(flags);
  printk("%s\n",_RTEMS_version);
  printk("FATAL ERROR:\n");
  printk("Internal error: %s\n", ISITNL? "Yes":"No");
  printk("Environment:");
  switch (THESRC) {
    case INTERNAL_ERROR_CORE:
      printk(" RTEMS Core\n");
      err = rtems_internal_error_description(THEERR);
    break;

      case INTERNAL_ERROR_RTEMS_API:
      printk(" RTEMS API\n");
      err = rtems_status_text(THEERR);
    break;

      case INTERNAL_ERROR_POSIX_API:
      printk(" POSIX API (errno)\n");
      /* could use strerror but I'd rather avoid using this here */
    break;

    default:
      printk("  UNKNOWN (0x%x)\n",THESRC);
  break;
  }
  if ( _Thread_Dispatch_is_enabled() )
    printk("enabled\n");
  else
    printk(
      "  Error occurred in a Thread Dispatching DISABLED context (level %i)\n",
      _Thread_Dispatch_get_disable_level());

  if ( _ISR_Nest_level ) {
    printk(
      "  Error occurred from ISR context (ISR nest level %i)\n",
      _ISR_Nest_level
    );
  }

  printk("Error %d",THEERR);
  if (err) {
    printk(": %s",err);
  }
  printk("\n");
  printk("Stack Trace:\n");
  CPU_print_stack();

  rebootQuestion();
}
