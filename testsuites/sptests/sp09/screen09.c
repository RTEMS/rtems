/*  Screen9
 *
 *  This routine generates error screen 9 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "system.h"

void Screen9()
{
  void              *converted;
  rtems_status_code status;
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
  puts( "TA1 - rtems_interrupt_catch - RTEMS_INVALID_ADDRESS" );

  status = rtems_signal_send( 100, RTEMS_SIGNAL_1 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_signal_send with illegal id"
  );
  puts( "TA1 - rtems_signal_send - RTEMS_INVALID_ID" );

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
  puts( "TA1 - rtems_port_create - RTEMS_INVALID_ADDRESS" );

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
  puts( "TA1 - rtems_port_external_to_internal - RTEMS_INVALID_ID" );
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
}
