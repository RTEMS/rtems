/*  intr.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Interrupt Manager.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_INTERRUPT_h
#define __RTEMS_INTERRUPT_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/core/isr.h>

/*
 *  Interrupt level type
 */

typedef ISR_Level rtems_interrupt_level;

/*
 *  The following type defines the control block used to manage
 *  the vectors.
 */
 
typedef ISR_Vector_number rtems_vector_number;

/*
 *  Return type for ISR Handler
 */
 
typedef void rtems_isr;

/*
 *  Pointer to an ISR Handler
 */
 
typedef rtems_isr ( *rtems_isr_entry )(
                 rtems_vector_number
             );

/*
 *  _Interrupt_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the interrupt manager.
 *
 */

void _Interrupt_Manager_initialization( void );

/*
 *  rtems_interrupt_catch
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_interrupt_catch directive.  This
 *  directive installs new_isr_handler as the RTEMS interrupt service
 *  routine for vector.  The previous RTEMS interrupt service
 *  routine is returned in old_isr_handler.
 */

rtems_status_code rtems_interrupt_catch(
  rtems_isr_entry    new_isr_handler,
  rtems_vector_number  vector,
  rtems_isr_entry   *old_isr_handler
);

/*
 *  rtems_interrupt_disable
 *
 *  DESCRIPTION:
 *
 *  This routine disables all maskable interrupts and returns the
 *  previous level in _isr_cookie.
 */

#define rtems_interrupt_disable( _isr_cookie ) \
    _ISR_Disable(_isr_cookie)
        
/*
 *  rtems_interrupt_enable
 *
 *  DESCRIPTION:
 *
 *  This routine enables maskable interrupts to the level indicated
 *  _isr_cookie.
 */

#define rtems_interrupt_enable( _isr_cookie ) \
    _ISR_Enable(_isr_cookie)
        
/*
 *  rtems_interrupt_flash
 *
 *  DESCRIPTION:
 *
 *  This routine temporarily enables maskable interrupts to the
 *  level in _isr_cookie before redisabling them.
 */

#define rtems_interrupt_flash( _isr_cookie ) \
    _ISR_Flash(_isr_cookie)

/*
 *  rtems_interrupt_cause
 *
 *  DESCRIPTION:
 *
 *  This routine generates an interrupt.
 *
 *  NOTE:  No implementation.
 */

#define rtems_interrupt_cause( _interrupt_to_cause )

/*
 *  rtems_interrupt_cause
 *
 *  DESCRIPTION:
 *
 *  This routine clears the specified interrupt.
 *
 *  NOTE:  No implementation.
 */

#define rtems_interrupt_clear( _interrupt_to_clear )

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
