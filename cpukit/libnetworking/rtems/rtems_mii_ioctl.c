/* Simple (default) implementation for SIOCGIFMEDIA/SIOCSIFMEDIA
 * to be used by ethernet drivers [from their ioctl].
 *
 * USERSPACE UTILITIES
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

#include <rtems.h>
#include <inttypes.h>

#undef _KERNEL

#include <rtems/rtems_mii_ioctl.h>

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static struct ifmedia_description shared_media_strings[] =
  IFM_SUBTYPE_SHARED_DESCRIPTIONS;
static struct ifmedia_description ethern_media_strings[] =
  IFM_SUBTYPE_ETHERNET_DESCRIPTIONS;
static struct ifmedia_description eth_al_media_strings[] =
  IFM_SUBTYPE_ETHERNET_ALIASES;

static const char *
find_desc (int tag, struct ifmedia_description *list)
{
  while (list->ifmt_string && tag != list->ifmt_word)
    list++;
  return list->ifmt_string;
}

#define WHATPRINT(buf,sz,fmt...)	\
	( (sz) > 0 ? snprintf((buf),(sz),fmt) : fprintf((buf) ? (FILE*)(buf) : stdout, fmt) )

int
rtems_ifmedia2str (int media, char *buf, int bufsz)
{
  const char *mdesc;
  const char *dupdesc = 0;

  /* only ethernet supported, so far */
  if (IFM_ETHER != IFM_TYPE (media))
    return -1;

  if (!(mdesc = find_desc (IFM_SUBTYPE (media), shared_media_strings)))
    mdesc = find_desc (IFM_SUBTYPE (media), ethern_media_strings);

  if (!mdesc)
    return -1;

  if (IFM_NONE != IFM_SUBTYPE (media))
    dupdesc = IFM_FDX & media ? " full-duplex" : " half-duplex";

  return WHATPRINT (buf, bufsz,
                    "Ethernet [phy instance: %" PRId32 "]: (link %s, autoneg %s) -- media: %s%s",
                    (int32_t) IFM_INST (media),
                    IFM_LINK_OK & media ? "ok" : "down",
                    IFM_ANEG_DIS & media ? "off" : "on",
                    mdesc, dupdesc ? dupdesc : "");
}

static int
find_tag (const char *desc, struct ifmedia_description *list)
{
  while (list->ifmt_string) {
    if (strstr (desc, list->ifmt_string))
      return list->ifmt_word;
    list++;
  }
  return -1;
}


/* convert a string to a media word
 * RETURNS: 0 on failure; valid results have always at least IFM_ETHER set
 */
int
rtems_str2ifmedia (const char *str, int phy)
{
  int sub, opt = 0;
  char *chpt;

  if (!strncmp (str, "auto", 4)) {
    sub = IFM_AUTO;
  } else if ((sub = find_tag (str, ethern_media_strings)) < 0) {
    if ((sub = find_tag (str, eth_al_media_strings)) < 0) {
      /* allow more */

      /* if no number, 0 is returned which will not pass the test */
      switch (strtol (str, &chpt, 10)) {
      case 10:
        sub = IFM_10_T;
        break;
      case 100:
        sub = IFM_100_TX;
        break;
      case 1000:
        sub = IFM_1000_T;
        break;
      default:
        return 0;
      }

      /* need 'b' or 'base' */
      if ('b' != *chpt++)
        return 0;
      if (!strncmp (chpt, "ase", 3))
        chpt += 3;
      if (toupper ((unsigned char)*chpt++) != 'T')
        return 0;
      if (IFM_100_TX == sub && toupper ((unsigned char)*chpt++) != 'X')
        return 0;
    }
  }

  if (strstr (str, "full") || strstr (str, "FDX") || strstr (str, "fdx"))
    opt |= IFM_FDX;

  return IFM_MAKEWORD (IFM_ETHER, sub, opt, phy);
}
