/*****************************************************************************/
/*
  $Id$

   CRC 16 Calculate Interface

*/
/*****************************************************************************/

#ifndef _CRC_H_
#define _CRC_H_

 /* ----
      F U N C T I O N S
  */
#if __cplusplus
extern "C" 
{
#endif

rtems_unsigned16 calc_crc(void *data, rtems_unsigned32 count);

#if __cplusplus
}
#endif
#endif
