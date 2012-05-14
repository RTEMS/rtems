/*  Screen9
 *
 *  This routine generates error screen 9 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

void Screen9()
{
  void              *converted;
  rtems_status_code status;

#if ((CPU_SIMPLE_VECTORED_INTERRUPTS == FALSE) || \
       defined(_C3x) || defined(_C4x))
    puts(
      "TA1 - rtems_interrupt_catch - "
      "bad handler RTEMS_INVALID_ADDRESS -- SKIPPED"
    );
    puts(
      "TA1 - rtems_interrupt_catch - "
      "old isr RTEMS_INVALID_ADDRESS - SKIPPED" );
#else
  rtems_isr_entry   old_service_routine;
    status = rtems_interrupt_catch(
      Service_routine,
      ISR_INTERRUPT_MAXIMUM_VECTOR_NUMBER + 10,
      &old_service_routine
    );
    fatal_directive_status(
      status,
      RTEMS_INVALID_NUMBER,
      "rtems_interrupt_catch with invalid vector"
    );
    puts( "TA1 - rtems_interrupt_catch - RTEMS_INVALID_NUMBER" );

    status = rtems_interrupt_catch( NULL, 3, &old_service_routine );
    fatal_directive_status(
      status,
      RTEMS_INVALID_ADDRESS,
      "rtems_interrupt_catch with invalid handler"
    );
    puts( "TA1 - rtems_interrupt_catch - bad handler RTEMS_INVALID_ADDRESS" );

    status = rtems_interrupt_catch( Service_routine, 3, NULL );
    fatal_directive_status(
      status,
      RTEMS_INVALID_ADDRESS,
      "rtems_interrupt_catch with invalid old isr pointer"
    );
    puts( "TA1 - rtems_interrupt_catch - old isr RTEMS_INVALID_ADDRESS" );
#endif

  /* send invalid id */
  status = rtems_signal_send( 100, RTEMS_SIGNAL_1 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_signal_send with illegal id"
  );
  puts( "TA1 - rtems_signal_send - RTEMS_INVALID_ID" );

  /* no signal in set */
  status = rtems_signal_send( RTEMS_SELF, 0 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_signal_send with no signals"
  );
  puts( "TA1 - rtems_signal_send - RTEMS_INVALID_NUMBER" );

  /* no signal handler */
  status = rtems_signal_send( RTEMS_SELF, RTEMS_SIGNAL_16 );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_signal_send with no handler"
  );
  puts( "TA1 - rtems_signal_send - RTEMS_NOT_DEFINED" );

  status = rtems_port_create(
     0,
     Internal_port_area,
     External_port_area,
     sizeof( Internal_port_area ),
     &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_port_create with illegal name"
  );
  puts( "TA1 - rtems_port_create - RTEMS_INVALID_NAME" );

#if defined(_C3x) || defined(_C4x)
  puts( "TA1 - rtems_port_create - RTEMS_INVALID_ADDRESS - SKIPPED" );
#else
  status = rtems_port_create(
     Port_name[ 1 ],
     &((char *)Internal_port_area)[ 1 ],
     External_port_area,
     sizeof( Internal_port_area ),
     &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_port_create with illegal address"
  );
  puts( "TA1 - rtems_port_create - bad range - RTEMS_INVALID_ADDRESS" );
#endif

  status = rtems_port_create(
     Port_name[ 1 ],
     Internal_port_area,
     External_port_area,
     sizeof( Internal_port_area ),
     NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_port_create null Id"
  );
  puts( "TA1 - rtems_port_create - null id - RTEMS_INVALID_ADDRESS" );

  status = rtems_port_create(
     Port_name[ 1 ],
     Internal_port_area,
     External_port_area,
     sizeof( Internal_port_area ),
     &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_port_create of too many"
  );
  puts( "TA1 - rtems_port_create - RTEMS_TOO_MANY" );

  status = rtems_port_delete( 0 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_port_delete with illegal id"
  );
  puts( "TA1 - rtems_port_delete - RTEMS_INVALID_ID" );

  status = rtems_port_ident( 0, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_port_ident with illegal name"
  );
  puts( "TA1 - rtems_port_ident - RTEMS_INVALID_NAME" );

  status = rtems_port_external_to_internal(
    100,
    Internal_port_area,
    &converted
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_port_external_to_internal with illegal id"
  );

  status = rtems_port_external_to_internal(
    100,
    Internal_port_area,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_port_external_to_internal with NULL param"
  );
  puts( "TA1 - rtems_port_external_to_internal - RTEMS_INVALID_ADDRESS" );

  status = rtems_port_internal_to_external(
    100,
    Internal_port_area,
    &converted
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_port_internal_to_external with illegal id"
  );
  puts( "TA1 - rtems_port_internal_to_external - RTEMS_INVALID_ID" );

  status = rtems_port_internal_to_external(
    100,
    Internal_port_area,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_port_internal_to_external with NULL param"
  );
  puts( "TA1 - rtems_port_external_to_internal - RTEMS_INVALID_ADDRESS" );
}
