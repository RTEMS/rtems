/*  Blackfin GPIO Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _gpioRegs_h_
#define _gpioRegs_h_


/* register addresses */

#define PORTIO_OFFSET                                 0x0000
#define PORTIO_CLEAR_OFFSET                           0x0004
#define PORTIO_SET_OFFSET                             0x0008
#define PORTIO_TOGGLE_OFFSET                          0x000c
#define PORTIO_MASKA_OFFSET                           0x0010
#define PORTIO_MASKA_CLEAR_OFFSET                     0x0014
#define PORTIO_MASKA_SET_OFFSET                       0x0018
#define PORTIO_MASKA_TOGGLE_OFFSET                    0x001c
#define PORTIO_MASKB_OFFSET                           0x0020
#define PORTIO_MASKB_CLEAR_OFFSET                     0x0024
#define PORTIO_MASKB_SET_OFFSET                       0x0028
#define PORTIO_MASKB_TOGGLE_OFFSET                    0x002c
#define PORTIO_DIR_OFFSET                             0x0030
#define PORTIO_POLAR_OFFSET                           0x0034
#define PORTIO_EDGE_OFFSET                            0x0038
#define PORTIO_BOTH_OFFSET                            0x003c
#define PORTIO_INEN_OFFSET                            0x0040


#endif /* _gpioRegs_h_ */

