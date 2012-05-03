/* MVME5500I2C.c
 *
 * Copyright (c) 2003, 2004 Brookhaven National Laboratory
 * Author:    S. Kate Feng <feng1@bnl.gov>
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution.
 *
 * To read information of the EEPROM via the I2C
 */

#include <rtems/bspIo.h>	    /* printk */
#include <stdint.h>           /* uint32_t */
#include "bsp/GT64260TWSI.h"

/* #define I2C_DEBUG*/

unsigned char I2cAddrPack(unsigned char busAddr,uint32_t offset)
{
  return(busAddr | ((offset & 0x700) >> 7));
}
unsigned char I2cDevByteAddr(uint32_t devA2A1A0, unsigned char byteNum)
{
  return(( devA2A1A0 >>(byteNum*8)) & 0xff);
}
/****************************************************************************
* I2Cread_eeprom - read EEPROM VPD from the I2C
*/
int I2Cread_eeprom(unsigned char I2cBusAddr,uint32_t devA2A1A0,uint32_t AddrBytes,unsigned char *pBuff,uint32_t numBytes)
{
  int status=0, lastByte=0;

  switch (AddrBytes) {
    case 1:
      if ((status=GT64260TWSIstart()) != -1) {
	if ((status=GT64260TWSIwrite(I2cAddrPack(I2cBusAddr,devA2A1A0)))!= -1){
	  if ((status=GT64260TWSIwrite(devA2A1A0))!=-1){
	    if ((status=GT64260TWSIstart())!=-1)
	        status=GT64260TWSIwrite(I2cAddrPack((I2cBusAddr|0x01),devA2A1A0));
	  }
        }
      }
      break;
    case 2:
      if ((status=GT64260TWSIstart())!=-1) {
	if ((status=GT64260TWSIwrite(I2cBusAddr))!= -1) {
	  if ((status=GT64260TWSIwrite(I2cDevByteAddr(devA2A1A0,1)))!=-1) {
	    if ((status=GT64260TWSIwrite(I2cDevByteAddr(devA2A1A0,0)))!= -1){
	      if ((status=GT64260TWSIstart()) != -1) {
		status = GT64260TWSIwrite((I2cBusAddr | 0x01));
              }
            }
          }
        }
      }
      break;
    case 3:
      if ((status = GT64260TWSIstart())!= -1) {
	if ((status = GT64260TWSIwrite(I2cBusAddr))!= -1) {
	  if ((status=GT64260TWSIwrite(I2cDevByteAddr(devA2A1A0,2)))!= -1){
	    if ((status=GT64260TWSIwrite(I2cDevByteAddr(devA2A1A0,1)))!= -1){
	      if ((status=GT64260TWSIwrite(I2cDevByteAddr(devA2A1A0,0)))!= -1){
  	        if ((status=GT64260TWSIstart())!= -1) {
		  status = GT64260TWSIwrite(I2cBusAddr | 0x01);
                }
              }
            }
          }
        }
      }
      break;
    default:
      status=-1;
      break;
  }
  if (status !=-1) {
#ifdef I2C_DEBUG
     printk("\n");
#endif
     /* read data from device */
     for ( ; numBytes > 0; numBytes-- ) {
       if ( numBytes == 1) lastByte=1;
       if (GT64260TWSIread(pBuff,lastByte) == -1) return (-1);
#ifdef I2C_DEBUG
       printk("%2x ", *pBuff);
       if ( (numBytes % 20)==0 ) printk("\n");
#endif
       pBuff++;
     }
#ifdef I2C_DEBUG
     printk("\n");
#endif
     if (GT64260TWSIstop() == -1) return (-1);
  }
  return (status);
}

