/*
 *  hw_init.c - initialize hardware resources for simulator.
 *  Currently there are no models of hardware resources which should be
 *  initialized here; it is just a stub.
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>

#include <stdlib.h>

#include <rtems/libio.h>
#include <iosupp.h>

/* exported entries */
extern void bsp_hw_init (void);
extern void early_hw_init (void);

/* early_hw_init --
 *     Hardware initialization called from 'start.S' on "#ifdef START_HW_INIT"
 *
 * ARGUMENTS:
 *     none
 *
 * RETURNS:
 *     none
 */
void
early_hw_init (void)
{
};

/* bsp_hw_init --
 *     Part of hardware initialization called from bspstart.c
 *
 * ARGUMENTS:
 *     none
 *
 * RETURNS:
 *     none
 */
void bsp_hw_init (void) 
{
}
