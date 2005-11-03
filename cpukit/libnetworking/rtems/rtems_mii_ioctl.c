/* $Id$ */

/* Simple (default) implementation for SIOCGIFMEDIA/SIOCSIFMEDIA
 * to be used by ethernet drivers [from their ioctl].
 *
 * USERSPACE UTILITIES
 *
 * NOTE: This much simpler than the BSD ifmedia API 
 */

/* Author: Till Straumann, <straumanatslacdotstandorddotedu>, 2005 */

#include <rtems.h>

#undef _KERNEL
#undef KERNEL

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
                    "Ethernet [phy instance: %i]: (link %s, autoneg %s) -- media: %s%s",
                    IFM_INST (media),
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
      if (toupper (*chpt++) != 'T')
        return 0;
      if (IFM_100_TX == sub && toupper (*chpt++) != 'X')
        return 0;
    }
  }

  if (strstr (str, "full") || strstr (str, "FDX") || strstr (str, "fdx"))
    opt |= IFM_FDX;

  return IFM_MAKEWORD (IFM_ETHER, sub, opt, phy);
}
