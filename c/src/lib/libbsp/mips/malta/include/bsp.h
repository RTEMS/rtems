/**
 *  @file
 *
 *  This include file contains some definitions specific to the
 *  MIPS Malta Board.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#define BSP_FEATURE_IRQ_EXTENSION
#define BSP_SHARED_HANDLER_SUPPORT      1

#define REVISION_REGISTER_ADDRESS  0x1fc00010
#define PRORV_MASK       0x0000000f   /* 4 bit Product Revision */
#define PROID_MASK       0x000000f0   /* 4 bit Product ID */
#define CORRV_MASK       0x00000300   /* 2 bit Core Board Revision */
#define CORID_MASK       0x0000fc00   /* 6 bit Core Board ID */
#define FPGRV_MASK       0x00ff0000   /* 8 bit CBUS FPGA Revision */
#define BSP_8259_BASE_ADDRESS    (0x18000000UL | 0xa0000000UL)
#define BSP_PCI_BASE_ADDRESS     (0x1be00000UL | 0xa0000000UL)
#define BSP_NIC_IO_BASE          (0x10000000UL | 0xa0000000UL)
#define PCI0_IO_BASE             (0x18000000UL | 0xa0000000UL)
#define BSP_NIC_MEM_BASE         (0x00000000UL | 0xa0000000UL)

/* functions */
#define WRITE_PROTECTED_UINT8( _addr, _value ) \
        do { \
          volatile uint8_t *_ptr = _addr | 0x80000000; \
          *_ptr = _value; \
        }
#define WRITE_PROTECTED_UINT16( _addr, _value ) \
        do { \
          volatile uint16_t *_ptr = _addr | 0x80000000; \
          *_ptr = _value; \
        }
#define WRITE_PROTECTED_UINT32( _addr, _value ) \
        do { \
          volatile uint32_t *_ptr = _addr | 0x80000000; \
          *_ptr = _value; \
        }
#define READ_PROTECTED_UINT8( _addr, _value ) \
        do { \
          volatile uint8_t *_ptr = _addr | 0x80000000; \
         _value = *_ptr; \
        }
#define READ_PROTECTED_UINT16( _addr, _value ) \
        do { \
          volatile uint16_t *_ptr = _addr | 0x80000000; \
         _value = *_ptr; \
        }
#define READ_PROTECTED_UINT32( _addr, _value ) \
        do { \
          volatile uint32_t *_ptr = _addr | 0x80000000; \
         _value = *_ptr; \
        }

#define READ_UINT8( _register_, _value_ ) \
        ((_value_) = *((volatile unsigned char *)(_register_)))

#define WRITE_UINT8( _register_, _value_ ) \
        (*((volatile unsigned char *)(_register_)) = (_value_))

#define READ_UINT16( _register_, _value_ ) \
     ((_value_) = *((volatile unsigned short *)(_register_)))

#define WRITE_UINT16( _register_, _value_ ) \
     (*((volatile unsigned short *)(_register_)) = (_value_))

void simple_out_32(uint32_t base, uint32_t addr, uint32_t val);
void simple_out_le32(uint32_t base, uint32_t addr, uint32_t val);
uint8_t simple_in_8( uint32_t base, uint32_t addr );
void simple_out_8( uint32_t base, uint32_t addr, uint8_t val );
int16_t simple_in_le16( uint32_t base, uint32_t addr );
int16_t simple_in_16( uint32_t base, uint32_t addr );
uint32_t simple_in_le32( uint32_t base, uint32_t addr );
uint32_t simple_in_32( uint32_t base, uint32_t addr );
void simple_out_le16( uint32_t base, uint32_t addr, uint16_t val );
void simple_out_16( uint32_t base, uint32_t addr, uint16_t val );

#ifdef __cplusplus
}
#endif

#endif
