/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <pcibios.h>

#include <string.h>	/* memcpy */

/*
 * This is simpliest possible PCI BIOS, it assumes that addressing
 * is flat and that stack is big enough 
 */ 


static int pcibInitialized = 0;
static unsigned int pcibEntry;

/*
 * Array to pass data between c and asm parts, at the time of 
 * writing I am not yet that familiar with extended asm feature 
 * of gcc. This code is not on performance path, so we can care 
 * relatively little about performance here 
 */
static volatile unsigned int pcibExchg[5];

static int pcib_convert_err(int err);

/*
 * Detects presense of PCI BIOS, returns 
 * error code
 */
int 
pcib_init(void)
{
  unsigned char *ucp;
  unsigned char sum;
  int      i;

  pcibInitialized = 0;

  /* First, we have to look for BIOS-32 */
  for(ucp=(unsigned char *)0xE0000; ucp < (unsigned char *)0xFFFFF; ucp+=0x10)
    {
      if(memcmp(ucp, "_32_", 4) != 0)
	{
	  continue;
	}

      /* Got signature, check length  */
      if(*(ucp + 9) != 1)
	{
	  continue;
	}

      /* Verify checksum */
      sum = 0;
      for(i=0; i<16; i++)
	{
	  sum += *(ucp+i);
	}

      if(sum == 0)
	{
	  /* found */
	  break;
	}
    }

  if(ucp >= (unsigned char *)0xFFFFF)
    {
      /* BIOS-32 not found */
      return PCIB_ERR_NOTPRESENT;
    }

  /* BIOS-32 found, let us find PCI BIOS */
  ucp += 4;

  pcibExchg[0] = *(unsigned int *)ucp;

  asm ("    pusha");                  /* Push all registers */  
  asm ("    movl pcibExchg, %edi");   /* Move entry point to esi */
  asm ("    movl $0x49435024, %eax"); /* Move signature to eax */
  asm ("    xorl %ebx, %ebx");        /* Zero ebx */
  asm ("    pushl %cs");
  asm ("    call *%edi");             /* Call entry */
  asm ("    movl %eax, pcibExchg");
  asm ("    movl %ebx, pcibExchg+4");
  asm ("    movl %ecx, pcibExchg+8");
  asm ("    movl %edx, pcibExchg+12");
  asm ("    popa");

  if((pcibExchg[0] & 0xff) != 0)
    {
      /* Not found */
      return PCIB_ERR_NOTPRESENT;
    }

  /* Found PCI entry point */
  pcibEntry = pcibExchg[1] + pcibExchg[3];

  /* Let us check whether PCI bios is present */
  pcibExchg[0] = pcibEntry;
  
  asm("    pusha");
  asm("    movl pcibExchg, %edi");
  asm("    movb $0xb1, %ah");
  asm("    movb $0x01, %al");
  asm ("   pushl %cs");
  asm("    call *%edi");
  asm("    movl %eax, pcibExchg");
  asm("    movl %ebx, pcibExchg+4");
  asm("    movl %ecx, pcibExchg+8");
  asm("    movl %edx, pcibExchg+12");
  asm("    popa");

  if((pcibExchg[0] & 0xff00) != 0)
    {
      /* Not found */
      return PCIB_ERR_NOTPRESENT;
    }

  if(pcibExchg[3] != 0x20494350)
    {
      /* Signature does not match */
      return PCIB_ERR_NOTPRESENT;
    }

  /* Success */
  
  pcibInitialized = 1;
  return PCIB_ERR_SUCCESS;
}

/* 
 * Find specified device and return its signature: combination 
 * of bus number, device number and function number
 */
int
pcib_find_by_devid(int vendorId, int devId, int idx, int *sig)
{
  if(!pcibInitialized)
    {
      return PCIB_ERR_UNINITIALIZED;
    }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = vendorId;
  pcibExchg[2] = devId;
  pcibExchg[3] = idx;

  asm("    pusha");
  asm("    movl pcibExchg, %edi");
  asm("    movb $0xb1, %ah");
  asm("    movb $0x02, %al");
  asm("    movl pcibExchg+4, %edx");
  asm("    movl pcibExchg+8, %ecx");
  asm("    movl pcibExchg+12, %esi");
  asm("    pushl %cs");
  asm("    call *%edi");
  asm("    movl %eax, pcibExchg");
  asm("    movl %ebx, pcibExchg+4");
  asm("    popa");

  *sig = pcibExchg[1] & 0xffff;

  return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
}

/* 
 * Find specified class code return device signature: combination 
 * of bus number, device number and function number
 */
int
pcib_find_by_class(int classCode, int idx, int *sig)
{
  if(!pcibInitialized)
    {
      return PCIB_ERR_UNINITIALIZED;
    }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = classCode;
  pcibExchg[2] = idx;

  asm("    pusha");
  asm("    movl pcibExchg, %edi");
  asm("    movb $0xb1, %ah");
  asm("    movb $0x03, %al");
  asm("    movl pcibExchg+4, %ecx");
  asm("    movl pcibExchg+8, %esi");
  asm("    pushl %cs");
  asm("    call *%edi");
  asm("    movl %eax, pcibExchg");
  asm("    movl %ebx, pcibExchg+4");
  asm("    popa");

  if((pcibExchg[0] & 0xff00) != 0)
    {
      return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
    }

  *sig = pcibExchg[1] & 0xffff;

  return PCIB_ERR_SUCCESS;
}
  
  


#define PCI_MULTI_FUNCTION       0x80
#define PCI_MAX_DEVICES		 16
#define PCI_MAX_FUNCTIONS	 8


int
BSP_pciFindDevice( unsigned short vendorid, unsigned short deviceid,
                   int instance, int *pbus, int *pdev, int *pfun )
{
   int sig;
   unsigned int d;
   unsigned short s;
   unsigned char bus,dev,fun,hd;

   for (bus=0; bus<BusCountPCI(); bus++) 
   {
      for (dev=0; dev<PCI_MAX_DEVICES; dev++) 
      {
         sig = PCIB_DEVSIG_MAKE(bus,dev,0);

         /* pci_read_config_byte(bus,dev,0, PCI_HEADER_TYPE, &hd); */
         pcib_conf_read8(sig, 0xe, &hd); 

         hd = (hd & PCI_MULTI_FUNCTION ? PCI_MAX_FUNCTIONS : 1);

         for (fun=0; fun<hd; fun++) {
            /* 
             * The last devfn id/slot is special; must skip it
             */
            if( PCI_MAX_DEVICES-1 == dev && PCI_MAX_FUNCTIONS-1 == fun )
               break;

            /*pci_read_config_dword(bus,dev,fun,PCI_VENDOR_ID,&d); */
            pcib_conf_read32(sig, 0, &d); 
            if( d == -1 )
               continue;
#ifdef PCI_DEBUG
            printk("BSP_pciFindDevice: found 0x%08x at %d/%d/%d\n",d,bus,dev,fun);
#endif
            /* pci_read_config_word(bus,dev,fun,PCI_VENDOR_ID,&s); */
            pcib_conf_read16(sig, 0, &s); 
            if (vendorid != s)
               continue;

            /* pci_read_config_word(bus,dev,fun,PCI_DEVICE_ID,&s); */
            pcib_conf_read16(sig, 0x2, &s); 
            if (deviceid == s) {
               if (instance--) continue;
               *pbus=bus; 
               *pdev=dev; 
               *pfun=fun;
               return 0;
            }
         }
      }
   }
   return -1;
}


  

/* 
 * Generate Special Cycle
 */
int
pcib_special_cycle(int busNo, int data)
{
  if(!pcibInitialized)
    {
      return PCIB_ERR_UNINITIALIZED;
    }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = busNo << 8;
  pcibExchg[2] = data;

  asm("    pusha");
  asm("    movl pcibExchg, %edi");
  asm("    movb $0xb1, %ah");
  asm("    movb $0x06, %al");
  asm("    movl pcibExchg+4, %ebx");
  asm("    movl pcibExchg+8, %edx");
  asm("    pushl %cs");
  asm("    call *%edi");
  asm("    movl %eax, pcibExchg");
  asm("    movl %ebx, pcibExchg+4");
  asm("    popa");

  return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
}
  

/* 
 * Read byte from config space
 */
int
pcib_conf_read8(int sig, int off, unsigned char *data)
{
  if(!pcibInitialized)
    {
      return PCIB_ERR_UNINITIALIZED;
    }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;

  asm("    pusha");
  asm("    movl pcibExchg, %esi");
  asm("    movb $0xb1, %ah");
  asm("    movb $0x08, %al");
  asm("    movl pcibExchg+4, %ebx");
  asm("    movl pcibExchg+8, %edi");
  asm("    pushl %cs");
  asm("    call *%esi");
  asm("    movl %eax, pcibExchg");
  asm("    movl %ecx, pcibExchg+4");
  asm("    popa");

  if((pcibExchg[0] & 0xff00) != 0)
    {
      return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
    }

  *data = (unsigned char)pcibExchg[1] & 0xff;

  return PCIB_ERR_SUCCESS;
}
  

/* 
 * Read word from config space
 */
int
pcib_conf_read16(int sig, int off, unsigned short *data)
{
  if(!pcibInitialized)
    {
      return PCIB_ERR_UNINITIALIZED;
    }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;

  asm("    pusha");
  asm("    movl pcibExchg, %esi");
  asm("    movb $0xb1, %ah");
  asm("    movb $0x09, %al");
  asm("    movl pcibExchg+4, %ebx");
  asm("    movl pcibExchg+8, %edi");
  asm("    pushl %cs");
  asm("    call *%esi");
  asm("    movl %eax, pcibExchg");
  asm("    movl %ecx, pcibExchg+4");
  asm("    popa");

  if((pcibExchg[0] & 0xff00) != 0)
    {
      return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
    }

  *data = (unsigned short)pcibExchg[1] & 0xffff;

  return PCIB_ERR_SUCCESS;
}
  

/* 
 * Read dword from config space
 */
int
pcib_conf_read32(int sig, int off, unsigned int *data)
{
  if(!pcibInitialized)
    {
      return PCIB_ERR_UNINITIALIZED;
    }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;

  asm("    pusha");
  asm("    movl pcibExchg, %esi");
  asm("    movb $0xb1, %ah");
  asm("    movb $0x0a, %al");
  asm("    movl pcibExchg+4, %ebx");
  asm("    movl pcibExchg+8, %edi");
  asm("    pushl %cs");
  asm("    call *%esi");
  asm("    movl %eax, pcibExchg");
  asm("    movl %ecx, pcibExchg+4");
  asm("    popa");

  if((pcibExchg[0] & 0xff00) != 0)
    {
      return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
    }

  *data = (unsigned int)pcibExchg[1];

  return PCIB_ERR_SUCCESS;
}
  

/* 
 * Write byte into  config space
 */
int
pcib_conf_write8(int sig, int off, unsigned int data)
{
  if(!pcibInitialized)
    {
      return PCIB_ERR_UNINITIALIZED;
    }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;
  pcibExchg[3] = data & 0xff;

  asm("    pusha");
  asm("    movl pcibExchg, %esi");
  asm("    movb $0xb1, %ah");
  asm("    movb $0x0b, %al");
  asm("    movl pcibExchg+4, %ebx");
  asm("    movl pcibExchg+8, %edi");
  asm("    movl pcibExchg+12, %ecx");
  asm("    pushl %cs");
  asm("    call *%esi");
  asm("    movl %eax, pcibExchg");
  asm("    popa");

  return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
}

/* 
 * Write word into config space
 */
int
pcib_conf_write16(int sig, int off, unsigned int data)
{
  if(!pcibInitialized)
    {
      return PCIB_ERR_UNINITIALIZED;
    }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;
  pcibExchg[3] = data & 0xffff;

  asm("    pusha");
  asm("    movl pcibExchg, %esi");
  asm("    movb $0xb1, %ah");
  asm("    movb $0x0c, %al");
  asm("    movl pcibExchg+4, %ebx");
  asm("    movl pcibExchg+8, %edi");
  asm("    movl pcibExchg+12, %ecx");
  asm("    pushl %cs");
  asm("    call *%esi");
  asm("    movl %eax, pcibExchg");
  asm("    popa");

  return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
}
  


/* 
 * Write dword into config space
 */
int
pcib_conf_write32(int sig, int off, unsigned int data)
{
  if(!pcibInitialized)
    {
      return PCIB_ERR_UNINITIALIZED;
    }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;
  pcibExchg[3] = data;

  asm("    pusha");
  asm("    movl pcibExchg, %esi");
  asm("    movb $0xb1, %ah");
  asm("    movb $0x0d, %al");
  asm("    movl pcibExchg+4, %ebx");
  asm("    movl pcibExchg+8, %edi");
  asm("    movl pcibExchg+12, %ecx");
  asm("    pushl %cs");
  asm("    call *%esi");
  asm("    movl %eax, pcibExchg");
  asm("    popa");

  return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
}
  

static int
pcib_convert_err(int err)
{
  switch(err & 0xff)
    {
    case 0:
      return PCIB_ERR_SUCCESS;
    case 0x81:
      return PCIB_ERR_NOFUNC;
    case 0x83:
      return PCIB_ERR_BADVENDOR;
    case 0x86:
      return PCIB_ERR_DEVNOTFOUND;
    case 0x87:
      return PCIB_ERR_BADREG;
    default:
      break;
    }
  return PCIB_ERR_NOFUNC;
}
	
      


  
      
  



