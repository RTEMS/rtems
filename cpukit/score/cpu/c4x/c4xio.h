/*
 *  C4X IO Information
 *
 *  $Id$
 */

#ifndef __C4XIO_h
#define __C4XIO_h

/*
 *  The following section of C4x timer code is based on C40 specific
 *  timer code from Ran Cabell <rcabell@norfolk.infi.net>.  The
 *  only C3x/C4x difference spotted was the address of the timer.
 *  The names have been changed to be more RTEMS like.
 */

struct c4x_timer {
    volatile int tcontrol;
    volatile int r1[3];
    volatile int tcounter;
    volatile int r2[3];
    volatile int tperiod;
};

#ifdef _TMS320C40
#define C4X_TIMER_0 ((struct c4x_timer*)0x100020)
#else
#define C4X_TIMER_0 ((struct c4x_timer*)0x808020)
#define C4X_TIMER_1 ((struct c4x_timer*)0x808030)
#endif

#define c4x_timer_start( _timer ) \
    _timer->tcontrol=0x02c1

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

#endif
/* end if include file */
