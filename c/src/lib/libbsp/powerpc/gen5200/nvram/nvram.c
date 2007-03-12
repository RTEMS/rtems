/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
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
| this file contains the nvram functions                          |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       nvram.c                                             */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS M93C64-based NV memory device driver          */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:  M93C46 is a serial microwire EEPROM which contains  */
/*                 1Kbit (128 bytes/64 words) of non-volatile memory.  */
/*                 The device can be coigured for byte- or word-       */
/*                 access. The driver provides a file-like interface   */
/*                 to this memory.                                     */
/*                                                                     */
/*                 MPC5x00 PIN settings:                               */
/*                                                                     */
/*                 PSC3_6 (output) -> MC93C46 serial data in    (D)    */
/*                 PSC3_7 (input)  -> MC93C46 serial data out   (Q)    */
/*                 PSC3_8 (output) -> MC93C46 chip select input (S)    */
/*                 PSC3_9 (output) -> MC93C46 serial clock      (C)    */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   DS1307-based Non-Volatile memory device driver      */
/*   Module:       nvram.c                                             */
/*   Project:      RTEMS 4.6.0pre1 / MCF5206Elite BSP                  */
/*   Version       1.2                                                 */
/*   Date:         11/04/2002                                          */
/*   Author:       Victor V. Vengerov                                  */
/*   Copyright:    Copyright (C) 2000 OKTET Ltd.,St.-Petersburg,Russia */
/*                                                                     */
/*   The license and distribution terms for this file may be           */
/*   found in the file LICENSE in this distribution or at              */
/*   http://www.rtems.com/license/LICENSE.                        */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Partially based on the code references which are named above.     */
/*   Adaptions, modifications, enhancements and any recent parts of    */
/*   the code are under the right of                                   */
/*                                                                     */
/*         IPR Engineering, Dachauer Straße 38, D-80335 München        */
/*                        Copyright(C) 2003                            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   IPR Engineering makes no representation or warranties with        */
/*   respect to the performance of this computer program, and          */
/*   specifically disclaims any responsibility for any damages,        */
/*   special or consequential, connected with the use of this program. */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Version history:  1.0                                             */
/*                                                                     */
/***********************************************************************/

#include <rtems.h>
#include <rtems/libio.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <bsp.h>
#include "../include/mpc5200.h"
#include "nvram.h"
#include "m93cxx.h"

/*
 * Simple usec delay function using lower half of HARPO Time Base Register
 */
void wait_usec(unsigned long usec)
  {
  unsigned long start_count = 0, update_count;
  unsigned long delay_count;

  if(TMBASE_CLOCK < 1000000)
    delay_count = (TMBASE_CLOCK * usec )/1000000;
  else
    delay_count = (TMBASE_CLOCK / 1000000) * usec;

  TBL_READ(start_count);

  update_count = start_count;

  while((update_count - start_count) < delay_count)
    TBL_READ(update_count);

  }


/*
 * Enable M93Cxx chip-write
 */
static void m93cxx_enable_write()
  {
  uint32_t header, i;

  ENABLE_CHIP_SELECT;

  WAIT(1);

  header = M93C46_EWEN;

  for(i = 0; i < M93C46_CLOCK_CYCLES; i++)
	{

	MASK_HEAD_SHIFT(header);

	WAIT(1);

	DO_CLOCK_CYCLE;

	WAIT(1);

	}

  DISABLE_CHIP_SELECT;

  return;

  }


/*
 * Disable M93Cxx chip-write
 */
static void m93cxx_disable_write()
  {
  uint32_t header, i;

  ENABLE_CHIP_SELECT;

  WAIT(1);

  header = M93C46_EWDS;

  for(i = 0; i < M93C46_CLOCK_CYCLES; i++)
	{

	MASK_HEAD_SHIFT(header);

	WAIT(1);

	DO_CLOCK_CYCLE;

	WAIT(1);

	}

  DISABLE_CHIP_SELECT;

  return;

  }


/*
 * Read one byte from specified offset
 */
static uint8_t m93cxx_read_byte(uint32_t offset)
  {
  uint8_t byte2read;
  uint32_t header, tmp_offset, i;
#ifdef M93CXX_MODE_BYTE
  uint8_t byte_recv = 0;
#else
	uint32_t word_recv = 0;
#endif

  ENABLE_CHIP_SELECT;

  WAIT(1);

#ifdef M93CXX_MODE_BYTE

  header = M93C46_READ(offset);

  for(i = 0; i < M93C46_CLOCK_CYCLES; i++)
	{

	MASK_HEAD_SHIFT(header);

	WAIT(1);

	DO_CLOCK_CYCLE;

	WAIT(1);

	}

  for(i = 0; i < 8; i++)
	{

    WAIT(1);

    DO_CLOCK_CYCLE;

    WAIT(1);

    GET_DATA_BYTE_SHIFT(byte_recv);

    }

  byte_recv >>= 1;

  byte2read = byte_recv;

#else
  tmp_offset = offset/2;

  header = M93C46_READ(tmp_offset);

  for(i = 0; i < M93C46_CLOCK_CYCLES; i++)
    {

    MASK_HEAD_SHIFT(header);

    WAIT(1);

    DO_CLOCK_CYCLE;

    WAIT(1);

    }

  for(i = 0; i < 16; i++)
    {

    DO_CLOCK_CYCLE;

    WAIT(1);

    GET_DATA_WORD_SHIFT(word_recv);

    WAIT(1);

    }

  word_recv >>= 1;

  if(offset%2)
	{

	byte2read = (uint8_t)((word_recv & 0xFF00) >> 8);

#ifdef NVRAM_DEBUG
	printf("\nbyte_read(o) = %x", byte2read);
#endif

	}
  else
	{

	byte2read = (uint8_t)(word_recv & 0x00FF);

#ifdef NVRAM_DEBUG
	printf("\nbyte_read(e) = %x", byte2read);
#endif
	}

#endif

  WAIT(1);

  DISABLE_CHIP_SELECT;

  return byte2read;

  }


/*
 * Write one byte to specified offset
 */
void m93cxx_write_byte(uint32_t offset, uint8_t byte2write)
  {
  uint32_t header, tmp_offset, i;
#ifdef M93CXX_MODE_BYTE
  uint8_t byte_send;
#else
  uint16_t word_send;
#endif

  ENABLE_CHIP_SELECT;

  WAIT(1);

#ifdef M93CXX_MODE_BYTE
  header = M93C46_WRITE(offset);

  for(i = 0; i < M93C46_CLOCK_CYCLES; i++)
	{

	MASK_HEAD_SHIFT(header);

	WAIT(1);

	DO_CLOCK_CYCLE;

	WAIT(1);

	}

  byte_send = byte2write;

  for(i = 0; i < 8; i++)
	{

	SET_DATA_BYTE_SHIFT(byte_send);

	WAIT(1);

	DO_CLOCK_CYCLE;

	WAIT(1);

	}

  }
#else

  if(offset%2)
	{

	word_send  = (uint16_t)m93cxx_read_byte(offset-1);
	word_send |= (uint16_t)(m93cxx_read_byte(offset) << 8);

	}
  else
	{

	word_send  = (uint16_t)m93cxx_read_byte(offset);
	word_send |= (uint16_t)(m93cxx_read_byte(offset + 1) << 8);

	}

  tmp_offset = offset/2;

  WAIT(1);

  ENABLE_CHIP_SELECT;

  WAIT(1);

  header = M93C46_WRITE(tmp_offset);

  for(i = 0; i < M93C46_CLOCK_CYCLES; i++)
	{

	MASK_HEAD_SHIFT(header);

	WAIT(1);

	DO_CLOCK_CYCLE;

	WAIT(1);

	}

  if(offset%2)
	{

	word_send  = (word_send & 0x00FF) | ((uint16_t)(byte2write << 8));

#ifdef NVRAM_DEBUG
	printf("\nword_send = %x", word_send);
#endif

	}
  else
	{

	word_send  = (word_send & 0xFF00) | (uint16_t)byte2write;
#ifdef NVRAM_DEBUG
	printf("\nword_send = %x", word_send);
#endif

	}

  for(i = 0; i < 16; i++)
	{

	SET_DATA_WORD_SHIFT(word_send);

	WAIT(1);

	DO_CLOCK_CYCLE;

	WAIT(1);

	}

  DISABLE_CHIP_SELECT;

  WAIT(1);

  ENABLE_CHIP_SELECT;

  WAIT(1);

  CHECK_WRITE_BUSY;

#endif

  WAIT(1);

  DISABLE_CHIP_SELECT;

  return;

  }


/* nvram_driver_initialize --
 *     Non-volatile memory device driver initialization.
 */
rtems_device_driver nvram_driver_initialize(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
  {
  rtems_status_code sc;

  /* enable PSC3_6/PSC3_7 as general purpose pins */
  mpc5200.gpiosen |= (GPIO_PSC3_6 | GPIO_PSC3_7);

  /* PSC3_6/PSC3_7 has normal CMOS output */
  mpc5200.gpiosod &= ~(GPIO_PSC3_6 | GPIO_PSC3_7);

  /* switch PSC3_6 (MC93C46 serial data in (D)) to low */
  mpc5200.gpiosdo &= ~GPIO_PSC3_6;

  /* PSC3_6 is an output (MC93C46 serial data in (D)) and PSC3_7 (MC93C46 serial data out (Q)) is an input pin */
  mpc5200.gpiosdd |= GPIO_PSC3_6;
  mpc5200.gpiosdd &= ~GPIO_PSC3_7;

  /* disable PSC3_8 interrupt capabilities */
  mpc5200.gpiosiie &= ~GPIO_PSC3_8;

  /* enable PSC3_8 as general purpose pin */
  mpc5200.gpiosie |= GPIO_PSC3_8;

  /* PSC3_8 has normal CMOS output */
  mpc5200.gpiosiod &= ~GPIO_PSC3_8;

  /* switch PSC3_8 (MC93C46 chip select input (S)) to low (high activ) */
  mpc5200.gpiosido &= ~GPIO_PSC3_8;

  /* PSC3_8 is an output (MC93C46 chip select input (S)) pin */
  mpc5200.gpiosidd |= GPIO_PSC3_8;

  /* disable PSC3_9 interrupt capabilities */
  mpc5200.gpiowue &= ~GPIO_PSC3_9;

  /* enable PSC3_9 as general purpose pins */
  mpc5200.gpiowe |= GPIO_PSC3_9;

  /* PSC3_9 has normal CMOS output */
  mpc5200.gpiowod &= ~GPIO_PSC3_9;

  /* switch PSC3_9 (MC93C46 serial clock (C)) to low */
  mpc5200.gpiowdo &= ~GPIO_PSC3_9;

  /* PSC3_9 is an output (MC93C46 serial clock (C)) pin */
  mpc5200.gpiowdd |= GPIO_PSC3_9;

  sc = rtems_io_register_name("/dev/nvram", major, 0);

  if(sc != RTEMS_SUCCESSFUL)
	{

	errno = EIO;
	/*errno = ENODEV;*/
	return RTEMS_UNSATISFIED;

	}
  else
	return RTEMS_SUCCESSFUL;

  }


/* nvram_driver_open --
 *     Non-volatile memory device driver open primitive.
 */
rtems_device_driver nvram_driver_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
  {

  return RTEMS_SUCCESSFUL;

  }


/* nvram_driver_close --
 *     Non-volatile memory device driver close primitive.
 */
rtems_device_driver nvram_driver_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
  {

  return RTEMS_SUCCESSFUL;

  }


/* nvram_driver_read --
 *     Non-volatile memory device driver read primitive.
 */
rtems_device_driver nvram_driver_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
  {
  rtems_libio_rw_args_t *args = arg;
  uint32_t count, i;

#ifdef NVRAM_DEBUG
  printf("\nread count  = %2x", (int)(args->count));
  printf("\nread offset = %2x", (int)(args->offset));
#endif

  if((args->offset >= M93C46_NVRAM_SIZE) || (args->offset + args->count > M93C46_NVRAM_SIZE))
    {

    args->bytes_moved = 0;
    errno = EINVAL;
    return RTEMS_UNSATISFIED;

    }
  else
    count = args->count;

  for(i = 0; i < count; i++)
    {

    (args->buffer)[i] = m93cxx_read_byte((args->offset) + i);
    (args->bytes_moved) += 1;

    }

  return RTEMS_SUCCESSFUL;

  }


/* nvram_driver_write --
 *     Non-volatile memory device driver write primitive.
 */
rtems_device_driver nvram_driver_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
  {
  rtems_libio_rw_args_t *args = arg;
  uint32_t count, i;

#ifdef NVRAM_DEBUG
  printf("\nwrite count  = %2x", (int)(args->count));
  printf("\nwrite offset = %2x", (int)(args->offset));
#endif

  if((args->offset >= M93C46_NVRAM_SIZE) || (args->offset + args->count > M93C46_NVRAM_SIZE))
    {

    args->bytes_moved = 0;
    errno = EINVAL;
    return RTEMS_UNSATISFIED;

    }
  else
    count = args->count;

    m93cxx_enable_write();

    WAIT(1);

  for(i = 0; i < count; i++)
    {

    m93cxx_write_byte((args->offset) + i, (args->buffer)[i]);
    (args->bytes_moved) += 1;

    }

  WAIT(1);

  m93cxx_disable_write();

  return RTEMS_SUCCESSFUL;

  }
