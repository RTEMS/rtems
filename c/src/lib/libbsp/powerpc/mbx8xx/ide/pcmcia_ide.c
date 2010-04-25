/*===============================================================*\
| Project: RTEMS MBX8xx PCMCIA IDE harddisc driver                |
+-----------------------------------------------------------------+
| File: pcmcia_ide.c                                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2003 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|               <Thomas.Doerfler@imd-systems.de>                  |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file contains the BSP layer for PCMCIA IDE access below the|
| libchip IDE harddisc driver                                     |
| based on a board specific driver from                           |
| Eugeny S. Mints, Oktet                                          |
|                                                                 |
|  The license and distribution terms for this file may be        |
|  found in the file LICENSE in this distribution or at           |
|  http://www.rtems.com/license/LICENSE.                     |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 01.14.03  creation                                         doe  |
\*===============================================================*/

#include <rtems.h>
#include <bsp.h>
#include <bsp/mbx.h>
#include <mpc8xx.h>
#include <libchip/ide_ctrl.h>
#include <libchip/ide_ctrl_cfg.h>
#include <libchip/ide_ctrl_io.h>

/* #define DATAREG_16BIT */ /* 16 bit mode not yet working */
/* #define DEBUG_OUT */
/*
 * support functions for PCMCIA IDE IF
 */
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
bool mbx8xx_pcmciaide_probe
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  This function should probe, whether a PCMCIA flash disk is available     |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int minor
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    true, when flash disk available                                        |
\*=========================================================================*/
{
  bool ide_card_plugged = true; /* assume: we have a card plugged in */

  /*
   * check, that the CD# pins are low -> a PCMCIA card is plugged in
   */
  if ((m8xx.pipr
       & (M8xx_PCMCIA_PIPR_CACD1 | M8xx_PCMCIA_PIPR_CACD2)) != 0x00) {
    ide_card_plugged = false;
  }
  /*
   * set supply voltage to 3.3V
   * FIXME: this should be depending on state of VS1/2 pins
   * FIXME: there should be a shadow variable for the BSP for CSR2 access
   */
  *((volatile uint8_t*)MBX_CSR2) = 0xb0;
  /*
   * check card information service whether card is a ATA like disk
   * -> scan for tuple of type 0x21 with content 0x04 0xXX (fixed disk)
   * -> scan for tuple of type 0x22 with content 0x01 0x01
   */
  if (ide_card_plugged) {
#define CIS_BYTE(pos) (((uint8_t*)PCMCIA_ATTRB_ADDR)[(pos)*2])
    int cis_pos = 0;
    bool fixed_disk_tuple_found = false;
    bool ata_disk_tuple_found   = false;

    while ((cis_pos < 256) &&
	   (CIS_BYTE(cis_pos) != 0xff) &&
	   (!fixed_disk_tuple_found || !ata_disk_tuple_found)) {
      /*
       * check for neede tuples
       */
      if ((CIS_BYTE(cis_pos  ) == 0x21) &&
	  (CIS_BYTE(cis_pos+2) == 0x04)) {
	fixed_disk_tuple_found = true;
      }
      else if ((CIS_BYTE(cis_pos  ) == 0x22) &&
	       (CIS_BYTE(cis_pos+2) == 0x01) &&
	       (CIS_BYTE(cis_pos+3) == 0x01)) {
	ata_disk_tuple_found = true;
      }
      /*
       * advance using the length field
       */
      cis_pos += CIS_BYTE(cis_pos+1)+2;
    }
    ide_card_plugged = fixed_disk_tuple_found && ata_disk_tuple_found;
  }
  return ide_card_plugged;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mbx8xx_pcmciaide_initialize
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  initialize PCMCIA IDE flash card access                                  |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int  minor                              /* controller minor number       */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  /*
   * FIXME: enable interrupts, if needed
   */
  /*
   * FIXME: set programming voltage as requested
   */
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mbx8xx_pcmciaide_read_reg
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  read a PCMCIA IDE controller register                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int                        minor,  /* controller minor number       */
 int                        reg,    /* register index to access      */
 uint16_t                  *value   /* ptr to return value location  */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  uint32_t    port = IDE_Controller_Table[minor].port1;

  if (reg == IDE_REGISTER_DATA_WORD) {
#ifdef DATAREG_16BIT
    *value = *(volatile uint16_t*)(port+reg);
#else
    *value = ((*(volatile uint8_t*)(port+reg) << 8) +
	      (*(volatile uint8_t*)(port+reg+1) ));
#endif
  }
  else {
    *value = *(volatile uint8_t*)(port+reg);
  }
#ifdef DEBUG_OUT
  printk("mbx8xx_pcmciaide_read_reg(0x%x)=0x%x\r\n",reg,*value & 0xff);
#endif
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mbx8xx_pcmciaide_write_reg
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  write a PCMCIA IDE controller register                                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int                        minor,  /* controller minor number       */
 int                        reg,    /* register index to access      */
 uint16_t                   value   /* value to write                */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  uint32_t    port = IDE_Controller_Table[minor].port1;

#ifdef DEBUG_OUT
  printk("mbx8xx_pcmciaide_write_reg(0x%x,0x%x)\r\n",reg,value & 0xff);
#endif
  if (reg == IDE_REGISTER_DATA_WORD) {
#ifdef DATAREG_16BIT
    *(volatile uint16_t*)(port+reg) = value;
#else
    *(volatile uint8_t*)(port+reg) = value >> 8;
    *(volatile uint8_t*)(port+reg+1) = value;
#endif
  }
  else {
    *(volatile uint8_t*)(port+reg)= value;
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mbx8xx_pcmciaide_read_block
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  read a PCMCIA IDE controller register                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int minor,
 uint32_t   block_size,
 rtems_blkdev_sg_buffer *bufs,
 uint32_t   *cbuf,
 uint32_t   *pos
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  uint32_t    port = IDE_Controller_Table[minor].port1;
  uint16_t    cnt = 0;
#ifdef DEBUG_OUT
  printk("mbx8xx_pcmciaide_read_block()\r\n");
#endif
#ifdef DATAREG_16BIT
  uint16_t   *lbuf = (uint16_t*)
    ((uint8_t*)(bufs[(*cbuf)].buffer) + (*pos));
#else
  uint8_t   *lbuf = (uint8_t*)
    ((uint8_t*)(bufs[(*cbuf)].buffer) + (*pos));
#endif
  uint32_t    llength = bufs[(*cbuf)].length;

  while (((*(volatile uint8_t*)(port+IDE_REGISTER_STATUS))
	  & IDE_REGISTER_STATUS_DRQ) &&
	 (cnt < block_size)) {
#ifdef DATAREG_16BIT
    *lbuf++ = *(volatile uint16_t*)(port+8); /* 16 bit data port */
    cnt += 2;
    (*pos) += 2;
#else
    *lbuf++ = *(volatile uint8_t*)(port+IDE_REGISTER_DATA);
    cnt += 1;
    (*pos) += 1;
#endif
    if ((*pos) == llength) {
      (*pos) = 0;
      (*cbuf)++;
      lbuf = bufs[(*cbuf)].buffer;
      llength = bufs[(*cbuf)].length;
    }
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mbx8xx_pcmciaide_write_block
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  write a PCMCIA IDE controller register                                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int minor,
 uint32_t   block_size,
 rtems_blkdev_sg_buffer *bufs,
 uint32_t   *cbuf,
 uint32_t   *pos
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  uint32_t    port = IDE_Controller_Table[minor].port1;
  uint16_t    cnt = 0;

#ifdef DEBUG_OUT
  printk("mbx8xx_pcmciaide_write_block()\r\n");
#endif
#ifdef DATA_REG_16BIT
  uint16_t   *lbuf = (uint16_t*)
    ((uint8_t*)(bufs[(*cbuf)].buffer) + (*pos));
#else
  uint8_t   *lbuf = (uint8_t*)
    ((uint8_t*)(bufs[(*cbuf)].buffer) + (*pos));
#endif
  uint32_t    llength = bufs[(*cbuf)].length;

  while (((*(volatile uint8_t*)(port+IDE_REGISTER_STATUS))
	  & IDE_REGISTER_STATUS_DRQ) &&
	 (cnt < block_size)) {
#ifdef DATAREG_16BIT
    *(volatile uint16_t*)(port+8) = *lbuf++; /* 16 bit data port */
    cnt += 2;
    (*pos) += 2;
#else
    *(volatile uint8_t*)(port+IDE_REGISTER_DATA) = *lbuf++;
    cnt += 1;
    (*pos) += 1;
#endif
    if ((*pos) == llength) {
      (*pos) = 0;
      (*cbuf)++;
      lbuf = bufs[(*cbuf)].buffer;
      llength = bufs[(*cbuf)].length;
    }
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int mbx8xx_pcmciaide_control
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  control interface for controller                                         |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int  minor,                        /* controller minor number       */
 uint32_t   cmd,                    /* command to send               */
 void * arg                         /* optional argument             */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  return 0;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code mbx8xx_pcmciaide_config_io_speed
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  set up transfer speed, if possible                                       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int        minor,                   /* controller minor number       */
 uint16_t   modes_avail              /* optional argument             */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  return RTEMS_SUCCESSFUL;
}

/*
 * The following table configures the functions used for IDE drivers
 * in this BSP.
 */

ide_ctrl_fns_t mbx8xx_pcmciaide_ctrl_fns = {
  mbx8xx_pcmciaide_probe,
  mbx8xx_pcmciaide_initialize,
  mbx8xx_pcmciaide_control,
  mbx8xx_pcmciaide_read_reg,
  mbx8xx_pcmciaide_write_reg,
  mbx8xx_pcmciaide_read_block,
  mbx8xx_pcmciaide_write_block,
  mbx8xx_pcmciaide_config_io_speed
};
