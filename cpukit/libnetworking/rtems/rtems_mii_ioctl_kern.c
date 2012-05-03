/* Simple (default) implementation for SIOCGIFMEDIA/SIOCSIFMEDIA
 * to be used by ethernet drivers [from their ioctl].
 *
 * KERNEL PART (support for drivers)
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

/* include first to avoid 'malloc' clash with rtems_bsdnet_malloc() hack */

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/ethernet.h>
#include <net/if.h>

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <rtems/rtems_mii_ioctl.h>

#include <errno.h>


#define DEBUG


#ifndef MII_1000TCR
#define MII_1000TCR MII_100T2CR
#endif

#ifndef MII_1000TSR
#define MII_1000TSR MII_100T2SR
#endif

int
rtems_mii_ioctl (struct rtems_mdio_info *info, void *uarg, uint32_t cmd,
                 int *media)
{
  uint32_t bmcr, bmsr, aner, bmcr2 = 0, bmsr2 = 0, anar, lpar;
  int phy = IFM_INST (*media);
  uint32_t tmp;
  int subtype = 0, options = 0;

  switch (cmd) {
  default:
    return EINVAL;

#ifdef DEBUG
  case 0:
#endif
  case SIOCGIFMEDIA:
    if (info->mdio_r (phy, uarg, MII_BMCR, &bmcr))
      return EINVAL;
	/* read BMSR twice to clear latched link status low */
    if (info->mdio_r (phy, uarg, MII_BMSR, &bmsr))
      return EINVAL;
    if (info->mdio_r (phy, uarg, MII_BMSR, &bmsr))
      return EINVAL;
    if (info->mdio_r (phy, uarg, MII_ANER, &aner))
      return EINVAL;
    if (info->has_gmii) {
      if (info->mdio_r (phy, uarg, MII_1000TCR, &bmcr2))
        return EINVAL;
      if (info->mdio_r (phy, uarg, MII_1000TSR, &bmsr2))
        return EINVAL;
    }

    /* link status */
    if (BMSR_LINK & bmsr)
      options |= IFM_LINK_OK;

    /* do we have autonegotiation disabled ? */
    if (!(BMCR_AUTOEN & bmcr)) {
      options |= IFM_ANEG_DIS;

      /* duplex is enforced */
      options |= BMCR_FDX & bmcr ? IFM_FDX : IFM_HDX;

      /* determine speed */
      switch (BMCR_SPEED (bmcr)) {
      case BMCR_S10:
        subtype = IFM_10_T;
        break;
      case BMCR_S100:
        subtype = IFM_100_TX;
        break;
      case BMCR_S1000:
        subtype = IFM_1000_T;
        break;
      default:
        return ENOTSUP;         /* ?? */
      }
    } else if (!(BMSR_LINK & bmsr) || !(BMSR_ACOMP & bmsr)) {
      subtype = IFM_NONE;
    } else {
      /* everything ok on our side */

	  if ( ! (ANER_LPAN & aner) ) {
		/* Link partner doesn't autonegotiate --> our settings are the
		 * result of 'parallel detect' (in particular: duplex status is HALF
		 * according to the standard!).
		 * Let them know that something's fishy...
		 */
		options |= IFM_ANEG_DIS;
	  }

      tmp = ((bmcr2 << 2) & bmsr2) & (GTSR_LP_1000THDX | GTSR_LP_1000TFDX);
      if (tmp) {
        if (GTSR_LP_1000TFDX & tmp)
          options |= IFM_FDX;
        subtype = IFM_1000_T;
      } else {
        if (info->mdio_r (phy, uarg, MII_ANAR, &anar))
          return EINVAL;
        if (info->mdio_r (phy, uarg, MII_ANLPAR, &lpar))
          return EINVAL;
        if (ANLPAR_ACK & lpar) {
          /* this is a negotiated link; otherwise we merely detect the partner's ability */
        }
        tmp = anar & lpar;
        if (ANLPAR_TX_FD & tmp) {
          options |= IFM_FDX;
          subtype = IFM_100_TX;
        } else if (ANLPAR_T4 & tmp) {
          subtype = IFM_100_T4;
        } else if (ANLPAR_TX & tmp) {
          subtype = IFM_100_TX;
        } else if (ANLPAR_10_FD & tmp) {
          options |= IFM_FDX;
          subtype = IFM_10_T;
        } else {
          subtype = IFM_10_T;
        }
      }
    }

    *media = IFM_MAKEWORD (IFM_ETHER, subtype, options, phy);

    break;

#ifdef DEBUG
  case 1:
#endif
  case SIOCSIFMEDIA:
    if (IFM_ETHER != IFM_TYPE (*media))
      return EINVAL;

    if (info->mdio_r (phy, uarg, MII_BMSR, &bmsr))
      return EINVAL;

    tmp = (IFM_FDX & *media);

    switch (IFM_SUBTYPE (*media)) {
    default:
      return ENOTSUP;

    case IFM_AUTO:
      bmcr = BMCR_AUTOEN | BMCR_STARTNEG;
      tmp = 0;
      break;

    case IFM_1000_T:
      if (!info->has_gmii)
        return ENOTSUP;

      if (info->mdio_r (phy, uarg, MII_EXTSR, &bmsr2))
        return EINVAL;

      if (!(bmsr2 & (tmp ? EXTSR_1000TFDX : EXTSR_1000THDX)))
        return EOPNOTSUPP;

	  /* NOTE: gige standard demands auto-negotiation for gige links.
	   *       Disabling autoneg did NOT work on the PHYs I tried
	   *       (BCM5421S, intel 82540).
	   *       I've seen drivers that simply change what they advertise
	   *       to the desired gig mode and re-negotiate.
	   *       We could do that here, too, but we don't see the point -
	   *       If autoneg works fine then we can as well use it.
	   */
      bmcr = BMCR_S1000;
      break;

    case IFM_100_TX:
      if (!(bmsr & (tmp ? BMSR_100TXFDX : BMSR_100TXHDX)))
        return EOPNOTSUPP;
      bmcr = BMCR_S100;
      break;

    case IFM_10_T:
      if (!(bmsr & (tmp ? BMSR_10TFDX : BMSR_10THDX)))
        return EOPNOTSUPP;
      bmcr = BMCR_S10;
      break;
    }

    if (tmp)
      bmcr |= BMCR_FDX;

    if (info->mdio_w (phy, uarg, MII_BMCR, bmcr))
      return EINVAL;

    /* TODO: should we adapt advertised capabilites ? */

    break;
  }

  return 0;
}
