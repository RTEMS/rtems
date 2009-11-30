/*===============================================================*\
| Project: RTEMS BSP support for TQ modules                       |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains definitions to interact with TQC's           |
|  processor modules                                              |
\*===============================================================*/

#ifndef __TQM_H__
#define __TQM_H__
#include <rtems.h>

typedef struct {
  uint32_t sdram_size;  /* existing SDRAM size */
  uint32_t flash_base;  /* start address flash */
  uint32_t flash_size;  /* existing Flash size */
  uint32_t flash_offset;
  uint32_t sram_base;   /* start address sram                */
  uint32_t sram_size;   /* existing sram size                */
  uint32_t immr_base;   /* start address internal memory map */
  uint32_t reboot;      /* reboot flag                       */
  uint8_t  ip_addr[4];  /* IP address                        */
  uint8_t  eth_addr[6]; /* ethernet (MAC) address            */
  uint8_t  gap_42[2];   /* gap for alignment                 */
  void (*put_char)(int c); /* function to output characters  */
} tqm_bd_info_t;

#define TQM_BD_INFO_ADDR 0x3400
#define TQM_BD_INFO (*(tqm_bd_info_t *)TQM_BD_INFO_ADDR)

#define TQM_CONF_INFO_BLOCK_ADDR 0x4001fe00

#define IMAP_ADDR		((unsigned int)0xfa200000)
#define IMAP_SIZE		((unsigned int)(64 * 1024))

#endif /* __TQM_H__ */
