/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __INTERRUPT_CRITICAL_SECTIONS_TEST_SUPPORT_h
#define __INTERRUPT_CRITICAL_SECTIONS_TEST_SUPPORT_h

/**
 *  @brief Initialize Test Support
 *
 *  @param[in] tsr is the optional timer service routine to fire
 */
void interrupt_critical_section_test_support_initialize(
  rtems_timer_service_routine (*tsr)( rtems_id, void * )
);

/**
 *  @brief Delay Test Support
 *
 *  This method delays a varying amount of time each call.
 *
 *  @return This method returns true each time the delay counter has
 *          to be reset.
 */
bool interrupt_critical_section_test_support_delay(void);

#endif

