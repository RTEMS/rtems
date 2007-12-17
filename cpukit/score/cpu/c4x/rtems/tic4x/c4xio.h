/*
 *  C4X IO Information
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_TIC4X_C4XIO_H
#define _RTEMS_TIC4X_C4XIO_H

/*
 *  Address defines
 */

#ifdef _TMS320C40
#define C4X_TIMER_0 ((struct c4x_timer*)0x100020)
#else
#define C4X_TIMER_0 ((struct c4x_timer*)0x808020)
#define C4X_TIMER_1 ((struct c4x_timer*)0x808030)
#endif

/* XXX how portable */

/* C32 Internal Control Registers */
#define C4X_STRB0_REG   0x808064
#define C4X_STRB1_REG   0x808068
#define C4X_IOSTRB_REG  0x808060

/* C32 Internal RAM Locations */
/* XXX how long */
#define C4X_RAM_BLK_0   0x87fe00
#define C4X_RAM_BLK_1   0x87ff00

/*
 *  Data Structures to Overlay the Peripherals on the CPU
 */

struct c4x_timer {
  volatile int tcontrol;
  volatile int r1[3];
  volatile int tcounter;
  volatile int r2[3];
  volatile int tperiod;
};




/*
 *  Timer Support Routines
 *
 *  The following section of C4x timer code is based on C40 specific
 *  timer code from Ran Cabell <rcabell@norfolk.infi.net>.  The
 *  only C3x/C4x difference spotted was the address of the timer.
 *  The names have been changed to be more RTEMS like.
 */

#define c4x_timer_get_control( _timer ) (volatile int)(_timer->tcontrol)

#define c4x_timer_set_control( _timer, _value ) \
  do { \
     (volatile int)(_timer->tcontrol) = _value; \
  } while (0);

#define c4x_timer_start( _timer ) \
    c4x_timer_set_control(_timer, 0x02c1 )

#define c4x_timer_stop( _timer ) _timer->tcontrol = 0

#define c4x_timer_get_counter( _timer ) (volatile int)(_timer->tcounter)

#define c4x_timer_set_counter( _timer, _value ) \
  do { \
     (volatile int)(_timer->tcounter) = _value; \
  } while (0);

#define c4x_timer_get_period( _timer ) (volatile int)(_timer->tperiod)

#define c4x_timer_set_period( _timer, _value ) \
  do { \
     (volatile int)(_timer->tperiod) = _value; \
  } while (0);

/*
 *  IO Flags
 *
 *  NOTE: iof on c3x, iiof on c4x 
 */

#ifdef _TMS320C40

#else

static inline uint32_t   c3x_get_iof( void )
{
  register uint32_t   iof_value;

  __asm__ volatile ("ldi iof, %0" : "=r" (iof_value));
  return iof_value;
}

static inline void c3x_set_iof( uint32_t   value )
{
  __asm__ volatile ("ldi  %0,iof" : : "g" (value) : "iof", "cc");
}

#endif


#endif
/* end if include file */
