/*  Screen9
 *
 *  This routine generates error screen 9 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  screen09.c,v 1.2 1995/05/31 17:09:07 joel Exp
 */

#include "system.h"

void Screen9()
{
  void              *converted;
  rtems_status_code status;
  rtems_isr_entry   old_service_routine;

  status = rtems_interrupt_catch( Service_routine, 500, &old_service_routine );
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
