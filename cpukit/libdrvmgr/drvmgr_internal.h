/* SPDX-License-Identifier: BSD-2-Clause */

/* Private driver manager declarations
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems/score/apimutex.h>

/*  Structure hold all information the driver manager needs to know of. Used
 *  internally by Driver Manager routines.
 */
struct drvmgr {
	int	level;
	int	initializing_objs;

	/* Device tree Lock */
	API_Mutex_Control	lock;

	/* The first device - The root device and it's driver */
	struct drvmgr_drv	*root_drv;
	struct drvmgr_dev	root_dev;

	/*!< Linked list of all registered drivers */
	struct drvmgr_list	drivers;

	/* Buses that reached a certain initialization level.
	 * Lists by Level:
	 *  N=0         - Not intialized, just registered
	 *  N=1..MAX-1  - Reached init level N
	 *  N=MAX       - Successfully initialized bus
	 */
	struct drvmgr_list	buses[DRVMGR_LEVEL_MAX+1];
	/* Buses failed to initialize or has been removed by not freed */
	struct drvmgr_list	buses_inactive;

	/* Devices that reached a certain initialization level.
	 * Lists by Level:
	 *  N=0         - Not intialized, just registered
	 *  N=1..MAX-1  - Reached init level N
	 *  N=MAX       - Successfully initialized device
	 */
	struct drvmgr_list	devices[DRVMGR_LEVEL_MAX+1];
	/*!< Devices failed to initialize, removed, ignored, no driver */
	struct drvmgr_list	devices_inactive;
};

extern struct drvmgr drvmgr;

extern void _DRV_Manager_Lock(void);
extern void _DRV_Manager_Unlock(void);

/* The best solution is to implement the locking with a RW lock, however there
 * is no such API available. Care must be taken so that dead-lock isn't created
 * for example in recursive functions.
 */
#if defined(DRVMGR_USE_LOCKS) && (DRVMGR_USE_LOCKS == 1)
 #define DRVMGR_LOCK_WRITE() _DRV_Manager_Lock()
 #define DRVMGR_LOCK_READ() _DRV_Manager_Lock()
 #define DRVMGR_UNLOCK() _DRV_Manager_Unlock()
#else
 /* no locking */
 #define DRVMGR_LOCK_WRITE() 
 #define DRVMGR_LOCK_READ() 
 #define DRVMGR_UNLOCK() 
#endif
