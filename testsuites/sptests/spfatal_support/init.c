/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

static void print_test_begin_message(void)
{
  static bool done = false;

  if (!done) {
    done = true;
    printk( "\n\n\n*** TEST FATAL " FATAL_ERROR_TEST_NAME " ***\n" );
  }
}

rtems_task Init(
  rtems_task_argument argument
)
{
  print_test_begin_message();
  force_error();
  printk( "Fatal error (%s) NOT hit\n", FATAL_ERROR_DESCRIPTION );
  rtems_test_exit(0);
}

char *Sources[] = {
  "INTERNAL_ERROR_CORE",
  "INTERNAL_ERROR_RTEMS_API",
  "INTERNAL_ERROR_POSIX_API",
};


char *Errors_Rtems[] = {
  "RTEMS_SUCCESSFUL",               /* successful completion */
  "RTEMS_TASK_EXITTED",             /* returned from a task */
  "RTEMS_MP_NOT_CONFIGURED",        /* multiprocessing not configured */
  "RTEMS_INVALID_NAME",             /* invalid object name */
  "RTEMS_INVALID_ID",               /* invalid object id */
  "RTEMS_TOO_MANY",                 /* too many */
  "RTEMS_TIMEOUT",                  /* timed out waiting */
  "RTEMS_OBJECT_WAS_DELETED",       /* object was deleted while waiting */
  "RTEMS_INVALID_SIZE",             /* specified size was invalid */
  "RTEMS_INVALID_ADDRESS",          /* address specified is invalid */
  "RTEMS_INVALID_NUMBER",           /* number was invalid */
  "RTEMS_NOT_DEFINED",              /* item has not been initialized */
  "RTEMS_RESOURCE_IN_USE",          /* resources still outstanding */
  "RTEMS_UNSATISFIED",              /* request not satisfied */
  "RTEMS_INCORRECT_STATE",          /* task is in wrong state */
  "RTEMS_ALREADY_SUSPENDED",        /* task already in state */
  "RTEMS_ILLEGAL_ON_SELF",          /* illegal operation on calling task */
  "RTEMS_ILLEGAL_ON_REMOTE_OBJECT", /* illegal operation for remote object */
  "RTEMS_CALLED_FROM_ISR",          /* called from ISR */
  "RTEMS_INVALID_PRIORITY",         /* invalid task priority */
  "RTEMS_INVALID_CLOCK",            /* invalid date/time */
  "RTEMS_INVALID_NODE",             /* invalid node id */
  "RTEMS_NOT_OWNER_OF_RESOURCE",    /* not owner of resource */
  "RTEMS_NOT_CONFIGURED",           /* directive not configured */
  "RTEMS_NOT_IMPLEMENTED"           /* directive not implemented */
};

char *Errors_Core[] = {
  "INTERNAL_ERROR_NO_CONFIGURATION_TABLE",
  "INTERNAL_ERROR_NO_CPU_TABLE",
  "INTERNAL_ERROR_TOO_LITTLE_WORKSPACE",
  "INTERNAL_ERROR_WORKSPACE_ALLOCATION",
  "INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL",
  "INTERNAL_ERROR_THREAD_EXITTED",
  "INTERNAL_ERROR_INCONSISTENT_MP_INFORMATION",
  "INTERNAL_ERROR_INVALID_NODE",
  "INTERNAL_ERROR_NO_MPCI",
  "INTERNAL_ERROR_BAD_PACKET",
  "INTERNAL_ERROR_OUT_OF_PACKETS",
  "INTERNAL_ERROR_OUT_OF_GLOBAL_OBJECTS",
  "INTERNAL_ERROR_OUT_OF_PROXIES",
  "INTERNAL_ERROR_INVALID_GLOBAL_ID",
  "INTERNAL_ERROR_BAD_STACK_HOOK",
  "INTERNAL_ERROR_BAD_ATTRIBUTES",
  "INTERNAL_ERROR_IMPLEMENTATION_KEY_CREATE_INCONSISTENCY",
  "INTERNAL_ERROR_IMPLEMENTATION_BLOCKING_OPERATION_CANCEL",
  "INTERNAL_ERROR_MUTEX_OBTAIN_FROM_BAD_STATE"
};

void Put_Error( uint32_t source, uint32_t error )
{
  if ( source == INTERNAL_ERROR_CORE ) {
    if ( error >  INTERNAL_ERROR_MUTEX_OBTAIN_FROM_BAD_STATE )
      printk("Unknown Internal Core Error (%d)", error);
    else
      printk( Errors_Core[ error ] );
  }
  else if (source == INTERNAL_ERROR_RTEMS_API ){
    if (error >  RTEMS_NOT_IMPLEMENTED )
      printk("Unknown Internal Rtems Error (0x%08x)", error);
    else
      printk( Errors_Rtems[ error ] );
  }
}

void Put_Source( uint32_t source )
{
  if ( source > INTERNAL_ERROR_POSIX_API )
    printk("Unknown Source (%d)", source);
  else
    printk( Sources[ source ] );
}


void Fatal_extension(
  uint32_t   source,
  bool       is_internal,
  uint32_t   error
)
{
  print_test_begin_message();
  printk( "Fatal error (%s) hit\n", FATAL_ERROR_DESCRIPTION );

  if ( source != FATAL_ERROR_EXPECTED_SOURCE ){
    printk( "ERROR==> Fatal Extension source Expected (");
    Put_Source( FATAL_ERROR_EXPECTED_SOURCE );
    printk( ") received (");
    Put_Source( source );
    printk( ")\n" );
  }

  if ( is_internal !=  FATAL_ERROR_EXPECTED_IS_INTERNAL )
  {
    if ( is_internal == TRUE )
      printk(
        "ERROR==> Fatal Extension is internal set to TRUE expected FALSE\n"
      );
    else
      printk(
        "ERROR==> Fatal Extension is internal set to FALSE expected TRUE\n"
      );
  }

  if ( error !=  FATAL_ERROR_EXPECTED_ERROR ) {
    printk( "ERROR==> Fatal Error Expected (");
    Put_Error( source, FATAL_ERROR_EXPECTED_ERROR );
    printk( ") received (");
    Put_Error( source, error );
    printk( ")\n" );
  }

  if (
    source == FATAL_ERROR_EXPECTED_SOURCE
      && is_internal == FATAL_ERROR_EXPECTED_IS_INTERNAL
      && error == FATAL_ERROR_EXPECTED_ERROR
  ) {
    printk( "*** END OF TEST FATAL " FATAL_ERROR_TEST_NAME " ***\n" );
  }

  if ( _System_state_Is_up( _System_state_Get() ) )
    _Thread_Stop_multitasking();
}

