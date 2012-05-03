/* Simple (default) implementation for SIOCGIFMEDIA/SIOCSIFMEDIA
 * to be used by ethernet drivers [from their ioctl].
 *
 * NOTE: This much simpler than the BSD ifmedia API
 */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2005,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */
#ifndef RTEMS_MII_IOCTL_H
#define RTEMS_MII_IOCTL_H

#include <dev/mii/mii.h>    /* MII register definitions                                */
#include <net/if_media.h>   /* media word definitions; rest of API (ifmedia) unused!   */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_KERNEL) || defined(KERNEL) || \
    defined(__KERNEL) || defined(__KERNEL__)
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
struct rtems_mdio_info {
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
rtems_mii_ioctl (struct rtems_mdio_info *info, void *uarg, uint32_t cmd,
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
 * INPUT:   if 'bufsz' is set to IFMEDIA2STR_PRINT_TO_FILE, 'buf' can be a FILE
 *          pointer where the info is printed insted. This can be NULL in which
 *          case 'stdout' is used.
 */

#define IFMEDIA2STR_PRINT_TO_FILE	0

int rtems_ifmedia2str (int media, char *buf, int bufsz);

/* convert a string to a media word
 * RETURNS: 0 on failure (unrecognized or invalid mode);
 *          valid results have always at least IFM_ETHER set.
 *
 * In addition to IFM_SUBTYPE_ETHERNET_DESCRIPTIONS and
 * IFM_SUBTYPE_ETHERNET_ALIASES, the strings
 *
 *  '10' [ '0' [ '0' ]] 'b' [ 'ase' ] ( 't' | 'T' )
 *           (* if 100bT [ 'x' | 'X' ] is required here *)
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
