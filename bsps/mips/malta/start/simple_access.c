/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  This file contains the code to do simple memory and io accesses.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems.h>
#include <bsp.h>

#include <bsp/pci.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>
#include <rtems/endian.h>
// #define DEBUG_ACCESSES   1

#ifdef DEBUG_ACCESSES
  #define JPRINTK(fmt, ...) printk("%s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#else
  #define JPRINTK(fmt, ...)
#endif

/*
 *  *  Simple accesses
 *   */
void simple_out_32(uint32_t base, uint32_t addr, uint32_t val)
{
  volatile uint32_t *ptr;

  ptr = (volatile uint32_t *) (base + addr);
  *ptr = val;

  JPRINTK( "%p data: 0x%x\n", ptr, val);
}

void simple_out_le32(uint32_t base, uint32_t addr, uint32_t val)
{
  volatile uint32_t *ptr;
  uint32_t           data = 0;

  ptr = (volatile uint32_t *) (base + addr);
  rtems_uint32_to_little_endian( val, (uint8_t *) &data);
  *ptr = data;

  JPRINTK( "%p data: 0x%x\n", ptr, data);
}

uint8_t simple_in_8( uint32_t base, uint32_t addr ) {
  volatile uint8_t *ptr;
  uint8_t           val;

  ptr = (volatile uint8_t *) (base + addr);
  val = *ptr;
  JPRINTK( "0x%x data: 0x%x\n", ptr, val);

  return val;
}

int16_t simple_in_le16( uint32_t base, uint32_t addr ) {
  volatile uint16_t *ptr;
  uint16_t           val;
  uint16_t           rval;

  ptr = (volatile uint16_t *) (base + addr);
  val = *ptr;
  rval = rtems_uint16_from_little_endian( (uint8_t *) &val);
  JPRINTK( "0x%x data: 0x%x raw: 0x%x\n", ptr, rval, val);
  return rval;
}

int16_t simple_in_16( uint32_t base, uint32_t addr ) {
  volatile uint16_t *ptr;
  uint16_t           val;

  ptr = (volatile uint16_t *) (base + addr);
  val = *ptr;
  JPRINTK( "0x%x data: 0x%x raw: 0x%x\n", ptr, val, val);
  return val;
}

uint32_t simple_in_le32( uint32_t base, uint32_t addr ) {
  volatile uint32_t *ptr;
  uint32_t           val;
  uint32_t           rval;

  ptr = (volatile uint32_t *) (base + addr);
  val = *ptr;
  rval = rtems_uint32_from_little_endian( (uint8_t *) &val);
  JPRINTK( "0x%x data: 0x%x raw: 0x%x\n", ptr, rval, val);
  return rval;
}

uint32_t simple_in_32( uint32_t base, uint32_t addr ) {
  volatile uint32_t *ptr;
  uint32_t           val;

  ptr = (volatile uint32_t *) (base + addr);
  val = *ptr;
  JPRINTK( "0x%x data: 0x%x raw: 0x%x\n", ptr, val, val);
  return val;
}

void simple_out_8( uint32_t base, uint32_t addr, uint8_t val ) {
  volatile uint8_t *ptr;

  ptr = (volatile uint8_t *) (base | addr);
  JPRINTK( "0x%x data: 0x%x\n", ptr, val);
  *ptr = val;
}

void simple_out_le16( uint32_t base, uint32_t addr, uint16_t val ) {
  volatile uint16_t *ptr;
  uint16_t           data;
  ptr = (volatile uint16_t *) (base + addr);
  rtems_uint16_to_little_endian( val, (uint8_t *) &data);
  *ptr = data;
  JPRINTK( "0x%x data: 0x%x\n", ptr, data);
}

void simple_out_16( uint32_t base, uint32_t addr, uint16_t val ) {
  volatile uint16_t *ptr;
  ptr = (volatile uint16_t *) (base + addr);
  *ptr = val;
  JPRINTK( "0x%x data: 0x%x\n", ptr, val);
}
