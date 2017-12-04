/* Private driver manager declarations
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
