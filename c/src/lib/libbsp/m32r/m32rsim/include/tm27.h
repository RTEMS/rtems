/**
 *  @file
 *
 *  @ingroup m32r_tm27
 *
 *  @brief Time Test 27
 */

/*
 *  tm27.h
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/**
 *  @defgroup m32r_tm27 Time Test 27
 *
 *  @ingroup m32r_m32rsim
 *
 *  @brief Define the interrupt mechanism for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) /* set_vector( (handler), 6, 1 ) */

#define Cause_tm27_intr()  /* XXX */

#define Clear_tm27_intr()  /* XXX */

#define Lower_tm27_intr() /* empty */

#endif
