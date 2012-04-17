/*
 *  AMBA Plug & Play routines
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <ambapp.h>

/* Calculate AHB Bus frequency of
 *   - Bus[0] (inverse=1), relative to the frequency of Bus[ahbidx]
 *     NOTE: set freq_hz to frequency of Bus[ahbidx].
 *   or
 *   - Bus[ahbidx] (inverse=0), relative to the frequency of Bus[0]
 *     NOTE: set freq_hz to frequency of Bus[0].
 *
 * If a unsupported bridge is found the invalid frequncy of 0Hz is
 * returned.
 */
unsigned int ambapp_freq_calc(
  struct ambapp_bus *abus,
  int ahbidx,
  unsigned int freq_hz,
  int inverse)
{
  struct ambapp_ahb_info *ahb;
  struct ambapp_dev *bridge;
  unsigned char ffact;
  int dir;

  /* Found Bus0? */
  bridge = abus->ahbs[ahbidx].bridge;
  if (!bridge)
    return freq_hz;

  /* Find this bus frequency relative to freq_hz */
  if ((bridge->vendor == VENDOR_GAISLER) &&
      ((bridge->device == GAISLER_AHB2AHB) ||
      (bridge->device == GAISLER_L2CACHE))) {
    ahb = DEV_TO_AHB(bridge);
    ffact = (ahb->custom[0] & AMBAPP_FLAG_FFACT) >> 4;
    if (ffact != 0) {
      dir = ahb->custom[0] & AMBAPP_FLAG_FFACT_DIR;

      /* Calculate frequency by dividing or
       * multiplying system frequency
       */
      if ((dir && !inverse) || (!dir && inverse))
        freq_hz = freq_hz * ffact;
      else
        freq_hz = freq_hz / ffact;
    }
    return ambapp_freq_calc(abus, ahb->ahbidx, freq_hz, inverse);
  } else {
    /* Unknown bridge, impossible to calc frequency */
    return 0;
  }
}

/* Find the frequency of all AHB Buses from knowing the frequency of one
 * particular APB/AHB Device.
 */
void ambapp_freq_init(
  struct ambapp_bus *abus,
  struct ambapp_dev *dev,
  unsigned int freq_hz)
{
  struct ambapp_common_info *info;
  int i;

  for (i=0; i<AHB_BUS_MAX; i++)
    abus->ahbs[i].freq_hz = 0;

  /* Register Frequency at the AHB bus that the device the user gave us
   * is located at.
   */
  if (dev) {
    info = DEV_TO_COMMON(dev);
    abus->ahbs[info->ahbidx].freq_hz = freq_hz;

    /* Find Frequency of Bus 0 */
    abus->ahbs[0].freq_hz = ambapp_freq_calc(abus, info->ahbidx, freq_hz, 1);
  } else {
    abus->ahbs[0].freq_hz = freq_hz;
  }

  /* Find Frequency of all except for Bus0 and the bus which frequency
   * was reported at
   */
  for (i=1; i<AHB_BUS_MAX; i++) {
    if (abus->ahbs[i].ioarea == 0)
      break;
    if (abus->ahbs[i].freq_hz != 0)
      continue;
    abus->ahbs[i].freq_hz = ambapp_freq_calc(abus, i, abus->ahbs[0].freq_hz, 0);
  }
}

/* Assign a AMBA Bus a frequency but reporting the frequency of a
 * particular AHB/APB device */
unsigned int ambapp_freq_get(struct ambapp_bus *abus, struct ambapp_dev *dev)
{
  struct ambapp_common_info *info = DEV_TO_COMMON(dev);
  return abus->ahbs[info->ahbidx].freq_hz;
}
