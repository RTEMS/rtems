#ifndef RTEMS_MII_IOCTL_H
#define RTEMS_MII_IOCTL_H

/* $Id$ */

/* Simple (default) implementation for SIOCGIFMEDIA/SIOCSIFMEDIA
 * to be used by ethernet drivers [from their ioctl].
 *
 * NOTE: This much simpler than the BSD ifmedia API 
 */

/* Author: Till Straumann, <straumanatslacdotstandorddotedu>, 2005 */

/* These should be moved out of <libchip>... */
#include <libchip/mii.h>        /* MII register definitions                                                                     */
#include <libchip/if_media.h>   /* media word definitions; rest of API (ifmedia) unused!        */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_KERNEL) || defined(KERNEL) || defined(__KERNEL) || defined(__KERNEL__)
/* mdio routines to be provided by driver */

/* read mii register 'reg' at 'phy' (-1 meaning any/currently active)
 * RETURNS 0 on success, -1 otherwise (e.g., illegal phy)
 */
typedef int (*rtems_mdio_read_func) (int phy, void *uarg, unsigned reg,
                                     uint32_t * pval);

/* write mii register 'reg' at 'phy' (-1 meaning any/currently active)
 * RETURNS 0 on success, -1 otherwise (e.g., illegal phy)
 */
typedef int (*rtems_mdio_write_func) (int phy, void *uarg, unsigned reg,
                                      uint32_t val);

/* Values to this must be provided by the driver */
struct rtems_mdio_info
{
  rtems_mdio_read_func mdio_r;
  rtems_mdio_write_func mdio_w;
  unsigned has_gmii:1;          /* supports gigabit */
};

/* Implement SIOCSIFMEDIA/SIOCGIFMEDIA; get/set the current media word. Note
 * that this does NOT implement the full BSD 'ifmedia' API; also, it only
 * implements IFM_ETHER...
 *
 * INPUT:
 *    SIOCGIFMEDIA: the media word must set the phy instance (-1 for 'any')
 *
 */
int
rtems_mii_ioctl (struct rtems_mdio_info *info, void *uarg, int cmd,
                 int *media);

#endif

/* The driver flags have the following meaning (SIOCGIFMEDIA only):
 */
#define IFM_LINK_OK		IFM_FLAG0
#define IFM_ANEG_DIS	IFM_FLAG1       /* autoneg. disabled; media forced */

/* convert a media word to a string;
 *
 * RETURNS: number of characters written to 'buf'
 *
 * INPUT:   if 'bufsz' is set to IFMEDIA2STR_PRINT_TO_FILE, 'buf' can be a FILE pointer
 *          where the info is printed insted. This can be NULL in which
 *          case 'stdout' is used.
 */

#define IFMEDIA2STR_PRINT_TO_FILE	0

int rtems_ifmedia2str (int media, char *buf, int bufsz);

/* convert a string to a media word
 * RETURNS: 0 on failure (unrecognized or invalid mode);
 *          valid results have always at least IFM_ETHER set.
 *
 * In addition to IFM_SUBTYPE_ETHERNET_DESCRIPTIONS and IFM_SUBTYPE_ETHERNET_ALIASES,
 * the strings
 *
 *  '10' [ '0' [ '0' ]] 'b' [ 'ase' ] ( 't' | 'T' ) (* if 100bT [ 'x' | 'X' ] is required here *)
 *
 * are recognized (e.g., 10bT, 100bTX)
 *
 * if any of the strings 'full' or 'FDX' or 'fdx' is present, a full-duplex mode
 * is selected (half-duplex otherwise).
 *  e.g., '100bTx-full'
 */

int rtems_str2ifmedia (const char *str, int phy);

#ifdef __cplusplus
}
#endif

#endif
