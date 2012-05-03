/*
 * RTEMS for Nintendo DS sgIP compatibility glue.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <rtems.h>
#include <stdlib.h>

#include <sys/mbuf.h>
#undef malloc
#undef free

#include "compat.h"

unsigned long sgIP_timems = 0;
static sgIP_Hub_HWInterface hw;

/*
 * Dswifi stub functions
 */

void *
sgIP_malloc (int size)
{
  return malloc (size);
}

void
sgIP_free (void *ptr)
{
  free (ptr);
}

/*
 * called by dswifi on initialization
 */

void
sgIP_Init (void)
{
}

/*
 * network layer stack timer called periodically
 */

void
sgIP_Timer (int num_ms)
{
  sgIP_timems += num_ms;
}

/*
 * add a new hardware interface
 * should be called once
 */

sgIP_Hub_HWInterface *
sgIP_Hub_AddHardwareInterface (int (*TransmitFunction)
                               (sgIP_Hub_HWInterface *, sgIP_memblock *),
                               int (*InterfaceInit) (sgIP_Hub_HWInterface *))
{
  hw.TransmitFunction = TransmitFunction;
  if (InterfaceInit)
    InterfaceInit (&hw);

  return &hw;
}

/*
 * callback on packet reception
 * translate memblock to mbuf
 */

extern void wifi_signal (struct mbuf *);
int
sgIP_Hub_ReceiveHardwarePacket (sgIP_Hub_HWInterface * hw,
                                sgIP_memblock * packet)
{
  struct mbuf *m;
  unsigned char *p;

  /* reserve the mbuf */
  MGETHDR (m, M_WAIT, MT_DATA);
  MCLGET (m, M_WAIT);

  /* copy data to mbuf */
  p = mtod (m, unsigned char *);
  m->m_len = m->m_pkthdr.len = packet->thislength;
  memcpy (p, packet->datastart, packet->thislength);

  /* free the memblock */
  sgIP_memblock_free (packet);

  /* signal packet arrival to the driver */
  wifi_signal (m);

  return 0;
}

/*
 * function for sending a packet (translates mbuf to memblock)
 */

void
compat_wifi_output (struct mbuf *m)
{
  sgIP_memblock *packet;
  size_t size = 0;
  struct mbuf *l;

  /* compute total size of the packet  */
  for (l = m; l != NULL; l = l->m_next)
    size += l->m_len;

  /* allocate memblock */
  packet = sgIP_memblock_allocHW (14, size - 14);

  /* copy data from mbuf to memblock */
  m_copydata (m, 0, size, packet->datastart);

  /* call TransmitFunction */
  hw.TransmitFunction (&hw, packet);

  /* free mbuf */
  m_freem (m);
}

/*
 * allocate a memblock
 */

sgIP_memblock *
sgIP_memblock_allocHW (int headersize, int packetsize)
{
  sgIP_memblock *mb;
  mb =
    (sgIP_memblock *) sgIP_malloc (SGIP_MEMBLOCK_HEADERSIZE +
                                   SGIP_MAXHWHEADER + packetsize);
  if (!mb)
    return 0;
  mb->totallength = headersize + packetsize;
  mb->thislength = mb->totallength;
  mb->datastart = mb->reserved + SGIP_MAXHWHEADER - headersize;
  mb->next = 0;
  return mb;
}

/*
 * release a memblock
 */

void
sgIP_memblock_free (sgIP_memblock * mb)
{
  sgIP_memblock *f;
  return;
  SGIP_INTR_PROTECT ();
  while (mb) {
    mb->totallength = 0;
    mb->thislength = 0;
    f = mb;
    mb = mb->next;

    sgIP_free (f);
  }

  SGIP_INTR_UNPROTECT ();
}

/*
 * start DHCP discovery
 */

void
sgIP_DHCP_Start (sgIP_Hub_HWInterface * interface, int getDNS)
{
}

/*
 * get DHCP state
 */

int
sgIP_DHCP_Update (void)
{
  return SGIP_DHCP_STATUS_SUCCESS;
}

/* ----------------- not my problem things ----------------  */

/* flush ARP table */
void
sgIP_ARP_FlushInterface (sgIP_Hub_HWInterface * hw)
{
}

/* send a ARP new IP notification */
int
sgIP_ARP_SendGratARP (sgIP_Hub_HWInterface * hw)
{
  return 0;
}

/* ----------------- unused things ------------------------ */

sgIP_DNS_Record *
sgIP_DNS_GetUnusedRecord (void)
{
  static sgIP_DNS_Record rec;

  return &rec;                  /* we must return a valid record even if not used */
}
