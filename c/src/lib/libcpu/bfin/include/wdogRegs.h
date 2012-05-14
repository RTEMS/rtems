/*  Blackfin Watchdog Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _wdogRegs_h_
#define _wdogRegs_h_

/* register addresses */

#define WDOG_CTL                (WDOG_BASE_ADDRESS + 0x0000)
#define WDOG_CNT                (WDOG_BASE_ADDRESS + 0x0004)
#define WDOG_STAT               (WDOG_BASE_ADDRESS + 0x0008)


/* register fields */

#define WDOG_CTL_WDRO                                 0x8000
#define WDOG_CTL_WDEN_MASK                            0x0ff0
#define WDOG_CTL_WDEN_DISABLE                         0x0ad0
#define WDOG_CTL_WDEV_MASK                            0x0006
#define WDOG_CTL_WDEV_RESET                           0x0000
#define WDOG_CTL_WDEV_NMI                             0x0002
#define WDOG_CTL_WDEV_GPI                             0x0004
#define WDOG_CTL_WDEV_DISABLE                         0x0006


#endif /* _wdogRegs_h_ */

