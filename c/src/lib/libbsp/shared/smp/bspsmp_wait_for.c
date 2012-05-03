/*
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

void rtems_bsp_delay( int usec );

void bsp_smp_wait_for(
  volatile unsigned int *address,
  unsigned int           desired,
  int                    maximum_usecs
)
{
  int iterations;
  volatile unsigned int *p = address;

  for (iterations=0 ;  iterations < maximum_usecs ; iterations++ ) {
    if ( *p == desired )
      break;

    rtems_bsp_delay( 1 );
  }
}
