/*
  ------------------------------------------------------------------------
  $Id$
  ------------------------------------------------------------------------
  
  My Right Boot, a boot ROM for embedded hardware.
 
  Copyright Cybertec Pty Ltd, 2000
  All rights reserved Cybertec Pty Ltd, 2000
  
  COPYRIGHT (c) 1989-1998.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution or at
  http://www.OARcorp.com/rtems/license.html.
  
  ------------------------------------------------------------------------

  CS8900 net boot driver.

 */

#include <bspIo.h>
#include <rtems.h>

#include <libchip/cs8900.h>

/*
 * Our local data.
 */

#ifdef CS8900_VERBOSE
static BOOLEAN cs8900_io_verbose;
#endif

static rtems_isr_void_entry old_handler[CS8900_DEVICES];
static void                 *old_parameter[CS8900_DEVICES];

/*
 * Tables of IO addresses and interrupt levels for each device attached.
 */

static const unsigned long ethernet_io_base[CS8900_DEVICES] =
{
  ETHERNET_BASE
};

static const unsigned long ethernet_mem_base[CS8900_DEVICES] =
{
  ETHERNET_BASE + CS8900_MEMORY_BASE
};

static const unsigned int ethernet_irq_level[CS8900_DEVICES] =
{
  ETHERNET_IRQ_LEVEL
};

static const unsigned int ethernet_irq_priority[CS8900_DEVICES] =
{
  ETHERNET_IRQ_PRIORITY
};

static const unsigned int ethernet_irq_vector[CS8900_DEVICES] =
{
  ETHERNET_IRQ_VECTOR,
};

void cs8900_io_set_reg (int dev, unsigned short reg, unsigned short data)
{
#ifdef CS8900_DATA_BUS_SWAPPED
  data = (data >> 8) | (data << 8);
#endif

#ifdef CS8900_VERBOSE
  if (cs8900_io_verbose)
    printf ("CS8900: io set reg=0x%04x, data=0x%04x\n", reg, data);
#endif
    
  WRITE_REGISTER_16 (ethernet_io_base[dev] + reg, data);
}

unsigned short cs8900_io_get_reg (int dev, unsigned short reg)
{
  unsigned long data;

  READ_REGISTER_16 (ethernet_io_base[dev] + reg, data);

#ifdef CS8900_DATA_BUS_SWAPPED
  data = (data >> 8) | (data << 8);
#endif
  
#ifdef CS8900_VERBOSE
  if (cs8900_io_verbose)
    printk ("CS8900: io get reg=0x%04x, data=0x%04x\n", reg, data);
#endif
  
  return data;
}

void cs8900_mem_set_reg (int dev, unsigned long reg, unsigned short data)
{
#ifdef CS8900_DATA_BUS_SWAPPED
  data = (data >> 8) | (data << 8);
#endif

#ifdef CS8900_VERBOSE
  if (cs8900_io_verbose)
    printk ("CS8900: mem set reg=0x%04x, data=0x%04x\n", reg, data);
#endif

  WRITE_REGISTER_16 (ethernet_io_base[dev] + reg, data);
}

unsigned short cs8900_mem_get_reg (int dev, unsigned long reg)
{
  unsigned short data;
  READ_REGISTER_16 (ethernet_io_base[dev] + reg, data);
  
#ifdef CS8900_DATA_BUS_SWAPPED
  data = (data >> 8) | (data << 8);
#endif

#ifdef CS8900_VERBOSE
  if (cs8900_io_verbose)
    printk ("CS8900: mem get reg=0x%04x, data=0x%04x\n", reg, data);
#endif

  return data;
}

void cs8900_put_data_block (int dev, int len, unsigned char *data)
{
#ifndef CS8900_DATA_BUS_SWAPPED
  unsigned short swap_word;
#endif
  unsigned short *src = (unsigned short *) ((unsigned long) data);
  unsigned short *dst = (unsigned short *) (ethernet_mem_base[dev] + CS8900_PP_TxFrameLoc);

  while (len > 1)
  {
#ifndef CS8900_DATA_BUS_SWAPPED
    swap_word = *src++;
    *dst++ = (swap_word >> 8) | (swap_word << 8);
#else
    *dst++ = *src++;
#endif
    len -= 2;
  }

  if (len)
  {
#ifndef CS8900_DATA_BUS_SWAPPED
    swap_word = *src++;
    *dst++ = (swap_word >> 8) | (swap_word << 8);
#else
    *dst++ = *src++;
#endif
  }  
}

unsigned short cs8900_get_data_block (int dev, unsigned char *data)
{
  unsigned short          swap_word;
  volatile unsigned short *src = (unsigned short *) (ethernet_mem_base[dev] + CS8900_PP_RxLength);
  unsigned short          *dst;
  unsigned short          len;
  unsigned short          rx_len;
  unsigned short          len_odd;
  
#ifdef CS8900_DATA_BUS_SWAPPED
    swap_word = *src++;
    len = (swap_word >> 8) | (swap_word << 8);
#else
    len = *src++;
#endif
    
  dst = (unsigned short *) ((unsigned long) data);

  len_odd = len & 1;
  rx_len  = len & ~1;
  
  for (; rx_len; rx_len -= 2)
  {
#ifndef CS8900_DATA_BUS_SWAPPED
    swap_word = *src++;
    *dst++ = (swap_word >> 8) | (swap_word << 8);
#else
    *dst++ = *src++;
#endif
  }

  if (len_odd)
  {
#ifndef CS8900_DATA_BUS_SWAPPED
    swap_word = *src++;
    *dst++ = (swap_word >> 8) | (swap_word << 8);
#else
    *dst++ = *src++;
#endif
  }

  return len;
}

void
cs8900_tx_load (int dev, struct mbuf *m)
{
  volatile unsigned short *dst = (unsigned short *) (ethernet_mem_base[dev] + CS8900_PP_TxFrameLoc);
  unsigned int            len;
  unsigned char           *src;
  int                     remainder = 0;
  unsigned char           remainder_data = '\0';
  
  while (m)
  {
    /*
     * We can get empty mbufs from the stack.
     */

    len = m->m_len;
    src = mtod (m, unsigned char*);

    if (len)
    {
      if (remainder)
      {
#ifndef CS8900_DATA_BUS_SWAPPED
        *dst++ = remainder_data | (*src++ << 8);
#else
        *dst++ = *src++ | (remainder_data << 8);
#endif
        len--;
        remainder = 0;
      }

      if (len & 1)
      {
        remainder = 1;
        len--;
      }
    
      for (; len; len -= 2)
#ifndef CS8900_DATA_BUS_SWAPPED
        *dst++ = (*src++) | (*(++src) << 8);
#else
      *dst++ = (*src++ << 8) | *(++src);
#endif
    
      if (remainder)
        remainder_data = *src++;
    }
    
    m = m->m_next;
  }

  if (remainder)
  {
#ifndef CS8900_DATA_BUS_SWAPPED
    *dst = (unsigned short) remainder_data;
#else
    *dst = (unsigned short) (remainder_data << 8);
#endif
  }
}

void cs8900_attach_interrupt (int dev, cs8900_device *cs)
{
  rtems_interrupt_catch_with_void (cs8900_interrupt, 
                                   ethernet_irq_vector[dev], 
                                   &old_handler[dev],
                                   cs,
                                   &old_parameter[dev]);
  
  CF_SIM_WRITE_ICR (CF_BASE, 
                    ethernet_irq_level[dev],
                    CF_SIM_ICR_AVEC_AUTO, 
                    ethernet_irq_level[dev], 
                    ethernet_irq_priority[dev]);
  CF_SIM_IMR_ENABLE (CF_BASE, 1 << ethernet_irq_level[dev]);
}

void cs8900_detach_interrupt (int dev)
{
  CF_SIM_IMR_DISABLE (CF_BASE, 1 << ethernet_irq_level[dev]);
  
  rtems_interrupt_catch_with_void (old_handler, 
                                   ethernet_irq_vector[dev], 
                                   NULL,
                                   old_parameter[dev],
                                   NULL);
}

void cs8900_get_mac_addr (int dev, unsigned char *mac_address)
{
  memcpy (mac_address, rct_get_mac_address (dev), 6);
}
