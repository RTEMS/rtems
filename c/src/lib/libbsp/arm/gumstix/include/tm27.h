/**
 * @file
 * @ingroup gumstix_tm27
 * @brief tm27 timing test support
 */

/*
 *  tm27.h
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/**
 * @defgroup gumstix_tm27 tm27 Support
 * @ingroup arm_gumstix
 * @brief tm27 Timing Test Support
 * @{
 */

/**
 * @name Interrupt mechanisms for Time Test 27
 * @{
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) /* empty */

#define Cause_tm27_intr() /* empty */

#define Clear_tm27_intr() /* empty */

#define Lower_tm27_intr() /* empty */

/** @} */

/** @} */

#endif
