/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCERC32
 *
 * @brief This file contains the implementation of the delay mechanisms for ERC32.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>

void rtems_bsp_delay(int usecs)
{
  uint32_t then;

  then  = ERC32_MEC.Real_Time_Clock_Counter;
  then += usecs;

  while (ERC32_MEC.Real_Time_Clock_Counter >= then)
    ;
}
