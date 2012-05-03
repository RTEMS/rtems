/*
 * RTEMS for Nintendo DS sgIP compatibility header.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#ifndef COMPAT_H_
#define COMPAT_H_

#include <rtems.h>

/* --------------- unused things --------------------- */

#define SGIP_DNS_FLAG_ACTIVE     1
#define SGIP_DNS_FLAG_RESOLVED   2
#define SGIP_DNS_FLAG_BUSY       4
#define SGIP_DNS_MAXRECORDADDRS              4
#define SGIP_DNS_MAXALIASES                  4

typedef struct SGIP_DNS_RECORD
{
  char name[256];
  char aliases[SGIP_DNS_MAXALIASES][256];
  unsigned char addrdata[SGIP_DNS_MAXRECORDADDRS * 4];
  short addrlen;
  short addrclass;
  int numaddr, numalias;
  int TTL;
  int flags;
} sgIP_DNS_Record;


/* -------------------------- */

#define SGIP_DEBUG_MESSAGE(param)
#define SGIP_DEBUG_ERROR(param)



#define SGIP_MAXHWADDRLEN	8
#define SGIP_MEMBLOCK_DATASIZE		1600



#define SGIP_MAXHWHEADER	16


#define SGIP_MEMBLOCK_HEADERSIZE 16
#define SGIP_MEMBLOCK_INTERNALSIZE (SGIP_MEMBLOCK_DATASIZE-16)
#define SGIP_MEMBLOCK_FIRSTINTERNALSIZE (SGIP_MEMBLOCK_DATASIZE-16-SGIP_MAXHWHEADER)
#define SGIP_INTR_PROTECT()
#define SGIP_INTR_REPROTECT()
#define SGIP_INTR_UNPROTECT()
#define SGIP_WAITEVENT();

typedef struct SGIP_MEMBLOCK
{
  int totallength;
  int thislength;
  struct SGIP_MEMBLOCK *next;
  char *datastart;
  char reserved[SGIP_MEMBLOCK_DATASIZE - 16];   // assume the other 4 values are 16 bytes total in length.
} sgIP_memblock;

typedef struct SGIP_HUB_HWINTERFACE
{
  unsigned short flags;
  unsigned short hwaddrlen;
  int MTU;
  int (*TransmitFunction) (struct SGIP_HUB_HWINTERFACE *, sgIP_memblock *);
  void *userdata;
  unsigned long ipaddr, gateway, snmask, dns[3];
  unsigned char hwaddr[SGIP_MAXHWADDRLEN];
} sgIP_Hub_HWInterface;

enum SGIP_DHCP_STATUS
{
  SGIP_DHCP_STATUS_IDLE,
  SGIP_DHCP_STATUS_WORKING,
  SGIP_DHCP_STATUS_FAILED,
  SGIP_DHCP_STATUS_SUCCESS
};

extern sgIP_Hub_HWInterface *wifi_hw;


/* prototypes */

sgIP_DNS_Record *sgIP_DNS_GetUnusedRecord (void);
void sgIP_DHCP_Start (sgIP_Hub_HWInterface * interface, int getDNS);
int sgIP_DHCP_Update (void);
int sgIP_ARP_SendGratARP (sgIP_Hub_HWInterface * hw);
void sgIP_Init (void);
void sgIP_Timer (int num_ms);
sgIP_Hub_HWInterface
  *sgIP_Hub_AddHardwareInterface (int (*TransmitFunction)
                                  (sgIP_Hub_HWInterface *, sgIP_memblock *),
                                  int (*InterfaceInit) (sgIP_Hub_HWInterface
                                                        *));

int sgIP_Hub_ReceiveHardwarePacket (sgIP_Hub_HWInterface * hw,
                                    sgIP_memblock * packet);
sgIP_memblock *sgIP_memblock_allocHW (int headersize, int packetsize);
void sgIP_memblock_free (sgIP_memblock * mb);
void sgIP_ARP_FlushInterface (sgIP_Hub_HWInterface * hw);

extern unsigned long sgIP_timems;

#endif
