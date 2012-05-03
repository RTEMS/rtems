#include <rtems.h>
#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/uart.h>
#include <rtems/bspIo.h>
#include <rtems/error.h>
#include <libcpu/stackTrace.h>
#include <rtems/score/thread.h>
#include <rtems/score/thread.inl>

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

char *score_status_text(rtems_status_code sc)
{
  switch (sc) {
    case INTERNAL_ERROR_NO_CONFIGURATION_TABLE:
      return "INTERNAL_ERROR_NO_CONFIGURATION_TABLE";
    case INTERNAL_ERROR_NO_CPU_TABLE:
      return "INTERNAL_ERROR_NO_CPU_TABLE";
    case INTERNAL_ERROR_TOO_LITTLE_WORKSPACE:
      return "INTERNAL_ERROR_TOO_LITTLE_WORKSPACE";
    case INTERNAL_ERROR_WORKSPACE_ALLOCATION:
      return "INTERNAL_ERROR_WORKSPACE_ALLOCATION";
    case INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL:
      return "INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL";
    case INTERNAL_ERROR_THREAD_EXITTED:
      return "INTERNAL_ERROR_THREAD_EXITTED";
    case INTERNAL_ERROR_INCONSISTENT_MP_INFORMATION:
      return "INTERNAL_ERROR_INCONSISTENT_MP_INFORMATION";
    case INTERNAL_ERROR_INVALID_NODE:
      return "INTERNAL_ERROR_INVALID_NODE";
    case INTERNAL_ERROR_NO_MPCI:
      return "INTERNAL_ERROR_NO_MPCI";
    case INTERNAL_ERROR_BAD_PACKET:
      return "INTERNAL_ERROR_BAD_PACKET";
    case INTERNAL_ERROR_OUT_OF_PACKETS:
      return "INTERNAL_ERROR_OUT_OF_PACKETS";
    case INTERNAL_ERROR_OUT_OF_GLOBAL_OBJECTS:
      return "INTERNAL_ERROR_OUT_OF_GLOBAL_OBJECTS";
    case INTERNAL_ERROR_OUT_OF_PROXIES:
      return "INTERNAL_ERROR_OUT_OF_PROXIES";
    case INTERNAL_ERROR_INVALID_GLOBAL_ID:
      return "INTERNAL_ERROR_INVALID_GLOBAL_ID";
    case INTERNAL_ERROR_BAD_STACK_HOOK:
      return "INTERNAL_ERROR_BAD_STACK_HOOK";
    case INTERNAL_ERROR_BAD_ATTRIBUTES:
      return "INTERNAL_ERROR_BAD_ATTRIBUTES";
    case 18: /* not in header (yet) :-( */
      return "INTERNAL_ERROR_CALLED_FROM_WRONG_ENVIRONMENT";
    default:
      break;
  }
  return 0;
}

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
      err = score_status_text(THEERR);
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
  if ( _Thread_Dispatch_in_critical_section() )
    printk(
      "  Error occurred in a Thread Dispatching DISABLED context (level %i)\n",
      _Thread_Dispatch_get_disable_level());
  else
    printk("enabled\n");

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
