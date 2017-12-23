#ifndef BSP_BSDNET_ATTACH_INFO_H
#define BSP_BSDNET_ATTACH_INFO_H

/* Author: Till Straumann, 2005; see ../../LICENSE */

/* Rationale: traditionally, BSPs only supported a single networking interface
 *            the BSP defined RTEMS_NETWORK_DRIVER_NAME & friends macros
 *            for applications to use.
 *            If more than one interface is present, this simple approach is
 *            not enough.
 *            Hence, this BSP exports a routine declaring all available interfaces
 *            so the application can make a choice.
 */

#ifdef __cplusplus
  extern "C" {
#endif

/* Fwd. decl just in case */
struct rtems_bsdnet_ifconfig;

typedef struct {
							/* name of the interface */
		const char *name;		
							/* optional description (to be used by chooser 'help' function etc.) */
		const char *description;
							/* driver 'attach' function */
		int			(*attach_fn)(struct rtems_bsdnet_ifconfig*, int);
} BSP_NetIFDescRec, *BSP_NetIFDesc;

/* Return a pointer to the (static) list of network interface descriptions
 * of this board.
 *
 * NOTES: A NULL value is returned if e.g., the board type cannot be determined
 *        or for other reasons.
 *        The 'description' field is optional, i.e., may be NULL.
 *        The list is terminated by an element with a NULL name field.
 *        The interfaces are listed in the order they are labelled.
 */

BSP_NetIFDesc
BSP_availableNetIFs();

/* Define this macro so applications can conditionally compile this API */
#define BSP_HAS_MULTIPLE_NETIFS(x)	BSP_availableNetIFs()	

/* Legacy macro; applications should use BSP_Available_NetIfs() to choose
 * an interface and attach function.
 */
extern char BSP_auto_network_driver_name[20];
#define RTEMS_BSP_NETWORK_DRIVER_NAME	BSP_auto_network_driver_name

#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	BSP_auto_enet_attach

/* This routine checks the name field passed in the 'ifconfig'.
 * If the name is NULL or points to the BSP_auto_network_driver_name
 * array, the routine checks all interfaces for an active link and
 * attaches the first alive one.
 * It also updates 'ifconfig' to reflect the chosen interface's name
 * and attach function.
 *
 * If another name is passed in, the routine scans
 * the available interfaces for that name and uses it, if found.
 * Eventually, a default interface is chosen (provided that
 * the board type is successfully detected).
 *
 * Note that only ONE interface chained into rtems_bsdnet_config
 * may use the "auto" name.
 *
 */

int
BSP_auto_enet_attach(struct rtems_bsdnet_ifconfig *ifconfig, int attaching);

#ifdef __cplusplus
  }
#endif

#endif
