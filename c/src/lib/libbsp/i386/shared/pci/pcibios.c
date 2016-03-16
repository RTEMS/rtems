/**
 * @file
 *
 * PCI Support when Configuration Space is accessed via BIOS
 */

/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/pci.h>

#include <string.h>  /* memcpy */

/*
 * This is simpliest possible PCI BIOS, it assumes that addressing
 * is flat and that stack is big enough
 */
static int pcibInitialized = 0;
static unsigned int pcibEntry;

/*
 * Array to pass data between c and __asm__ parts, at the time of
 * writing I am not yet that familiar with extended __asm__ feature
 * of gcc. This code is not on performance path, so we can care
 * relatively little about performance here
 */
static volatile unsigned int pcibExchg[5];

static int pcib_convert_err(int err);

/** @brief
 * Make device signature from bus number, device numebr and function
 * number
 */
#define PCIB_DEVSIG_MAKE(b,d,f) ((b<<8)|(d<<3)|(f))

/** @brief
 * Extract valrous part from device signature
 */
#define PCIB_DEVSIG_BUS(x) (((x)>>8) &0xff)
#define PCIB_DEVSIG_DEV(x) (((x)>>3) & 0x1f)
#define PCIB_DEVSIG_FUNC(x) ((x) & 0x7)

/*
 * Forward reference. Initialized at bottom.
 */
static const pci_config_access_functions pci_bios_indirect_functions;

/* prototype before defining */
const pci_config_access_functions *pci_bios_initialize(void);

/*
 * Detects presense of PCI BIOS, returns pointer to accessor methods.
 */
const pci_config_access_functions *pci_bios_initialize(void)
{
  unsigned char *ucp;
  unsigned char  sum;
  int            i;

  pcibInitialized = 0;

  /* First, we have to look for BIOS-32 */
  for (ucp = (unsigned char *)0xE0000;
       ucp < (unsigned char *)0xFFFFF;
       ucp += 0x10) {
    if (memcmp(ucp, "_32_", 4) != 0) {
      continue;
    }

      /* Got signature, check length  */
    if (*(ucp + 9) != 1) {
      continue;
    }

    /* Verify checksum */
    sum = 0;
    for (i=0; i<16; i++) {
      sum += *(ucp+i);
    }

    if (sum == 0) {
      /* found */
      break;
    }
  }

  if (ucp >= (unsigned char *)0xFFFFF) {
    /* BIOS-32 not found */
    return NULL;
  }

  /* BIOS-32 found, let us find PCI BIOS */
  ucp += 4;

  pcibExchg[0] = *(unsigned int *)ucp;

  __asm__ ("    pusha");                  /* Push all registers */
  __asm__ ("    movl pcibExchg, %edi");   /* Move entry point to esi */
  __asm__ ("    movl $0x49435024, %eax"); /* Move signature to eax */
  __asm__ ("    xorl %ebx, %ebx");        /* Zero ebx */
  __asm__ ("    pushl %cs");
  __asm__ ("    call *%edi");             /* Call entry */
  __asm__ ("    movl %eax, pcibExchg");
  __asm__ ("    movl %ebx, pcibExchg+4");
  __asm__ ("    movl %ecx, pcibExchg+8");
  __asm__ ("    movl %edx, pcibExchg+12");
  __asm__ ("    popa");

  if ((pcibExchg[0] & 0xff) != 0) {
    /* Not found */
    return NULL;
  }

  /* Found PCI entry point */
  pcibEntry = pcibExchg[1] + pcibExchg[3];

  /* Let us check whether PCI bios is present */
  pcibExchg[0] = pcibEntry;

  __asm__ ("    pusha");
  __asm__ ("    movl pcibExchg, %edi");
  __asm__ ("    movb $0xb1, %ah");
  __asm__ ("    movb $0x01, %al");
  __asm__ ("    pushl %cs");
  __asm__ ("    call *%edi");
  __asm__ ("    movl %eax, pcibExchg");
  __asm__ ("    movl %ebx, pcibExchg+4");
  __asm__ ("    movl %ecx, pcibExchg+8");
  __asm__ ("    movl %edx, pcibExchg+12");
  __asm__ ("    popa");

  if ((pcibExchg[0] & 0xff00) != 0) {
    /* Not found */
    return NULL;
  }

  if (pcibExchg[3] != 0x20494350) {
    /* Signature does not match */
    return NULL;
  }

  /* Success */
  pcibInitialized = 1;

  return &pci_bios_indirect_functions;
}

/*
 * Read byte from config space
 */
static int
pcib_conf_read8(int sig, int off, uint8_t *data)
{
  if (!pcibInitialized) {
    return PCIB_ERR_UNINITIALIZED;
  }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;

  __asm__ ("    pusha");
  __asm__ ("    movl pcibExchg, %esi");
  __asm__ ("    movb $0xb1, %ah");
  __asm__ ("    movb $0x08, %al");
  __asm__ ("    movl pcibExchg+4, %ebx");
  __asm__ ("    movl pcibExchg+8, %edi");
  __asm__ ("    pushl %cs");
  __asm__ ("    call *%esi");
  __asm__ ("    movl %eax, pcibExchg");
  __asm__ ("    movl %ecx, pcibExchg+4");
  __asm__ ("    popa");

  if ((pcibExchg[0] & 0xff00) != 0) {
    return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
  }

  *data = (unsigned char)pcibExchg[1] & 0xff;

  return PCIB_ERR_SUCCESS;
}


/*
 * Read word from config space
 */
static int
pcib_conf_read16(int sig, int off, uint16_t *data)
{
  if (!pcibInitialized) {
    return PCIB_ERR_UNINITIALIZED;
  }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;

  __asm__ ("    pusha");
  __asm__ ("    movl pcibExchg, %esi");
  __asm__ ("    movb $0xb1, %ah");
  __asm__ ("    movb $0x09, %al");
  __asm__ ("    movl pcibExchg+4, %ebx");
  __asm__ ("    movl pcibExchg+8, %edi");
  __asm__ ("    pushl %cs");
  __asm__ ("    call *%esi");
  __asm__ ("    movl %eax, pcibExchg");
  __asm__ ("    movl %ecx, pcibExchg+4");
  __asm__ ("    popa");

  if ((pcibExchg[0] & 0xff00) != 0) {
    return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
  }

  *data = (unsigned short)pcibExchg[1] & 0xffff;

  return PCIB_ERR_SUCCESS;
}


/*
 * Read dword from config space
 */
static int
pcib_conf_read32(int sig, int off, uint32_t *data)
{
  if (!pcibInitialized) {
    return PCIB_ERR_UNINITIALIZED;
  }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;

  __asm__ ("    pusha");
  __asm__ ("    movl pcibExchg, %esi");
  __asm__ ("    movb $0xb1, %ah");
  __asm__ ("    movb $0x0a, %al");
  __asm__ ("    movl pcibExchg+4, %ebx");
  __asm__ ("    movl pcibExchg+8, %edi");
  __asm__ ("    pushl %cs");
  __asm__ ("    call *%esi");
  __asm__ ("    movl %eax, pcibExchg");
  __asm__ ("    movl %ecx, pcibExchg+4");
  __asm__ ("    popa");

  if ((pcibExchg[0] & 0xff00) != 0) {
    return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
  }

  *data = (unsigned int)pcibExchg[1];

  return PCIB_ERR_SUCCESS;
}


/*
 * Write byte into  config space
 */
static int
pcib_conf_write8(int sig, int off, uint8_t data)
{
  if (!pcibInitialized) {
    return PCIB_ERR_UNINITIALIZED;
  }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;
  pcibExchg[3] = data & 0xff;

  __asm__ ("    pusha");
  __asm__ ("    movl pcibExchg, %esi");
  __asm__ ("    movb $0xb1, %ah");
  __asm__ ("    movb $0x0b, %al");
  __asm__ ("    movl pcibExchg+4, %ebx");
  __asm__ ("    movl pcibExchg+8, %edi");
  __asm__ ("    movl pcibExchg+12, %ecx");
  __asm__ ("    pushl %cs");
  __asm__ ("    call *%esi");
  __asm__ ("    movl %eax, pcibExchg");
  __asm__ ("    popa");

  return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
}

/*
 * Write word into config space
 */
static int
pcib_conf_write16(int sig, int off, uint16_t data)
{
  if (!pcibInitialized) {
    return PCIB_ERR_UNINITIALIZED;
  }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;
  pcibExchg[3] = data & 0xffff;

  __asm__ ("    pusha");
  __asm__ ("    movl pcibExchg, %esi");
  __asm__ ("    movb $0xb1, %ah");
  __asm__ ("    movb $0x0c, %al");
  __asm__ ("    movl pcibExchg+4, %ebx");
  __asm__ ("    movl pcibExchg+8, %edi");
  __asm__ ("    movl pcibExchg+12, %ecx");
  __asm__ ("    pushl %cs");
  __asm__ ("    call *%esi");
  __asm__ ("    movl %eax, pcibExchg");
  __asm__ ("    popa");

  return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
}



/*
 * Write dword into config space
 */
static int
pcib_conf_write32(int sig, int off, uint32_t data)
{
  if (!pcibInitialized){
      return PCIB_ERR_UNINITIALIZED;
  }

  pcibExchg[0] = pcibEntry;
  pcibExchg[1] = sig;
  pcibExchg[2] = off;
  pcibExchg[3] = data;

  __asm__ ("    pusha");
  __asm__ ("    movl pcibExchg, %esi");
  __asm__ ("    movb $0xb1, %ah");
  __asm__ ("    movb $0x0d, %al");
  __asm__ ("    movl pcibExchg+4, %ebx");
  __asm__ ("    movl pcibExchg+8, %edi");
  __asm__ ("    movl pcibExchg+12, %ecx");
  __asm__ ("    pushl %cs");
  __asm__ ("    call *%esi");
  __asm__ ("    movl %eax, pcibExchg");
  __asm__ ("    popa");

  return pcib_convert_err((pcibExchg[0] >> 8) & 0xff);
}


static int
pcib_convert_err(int err)
{
  switch(err & 0xff){
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

static int
BSP_pci_read_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char fun,
  unsigned char offset,
  unsigned char *val
)
{
  int sig;

  sig = PCIB_DEVSIG_MAKE(bus,slot,fun);
  pcib_conf_read8(sig, offset, val);
  return PCIBIOS_SUCCESSFUL;
}

static int
BSP_pci_read_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char fun,
  unsigned char offset,
  unsigned short *val
)
{
  int sig;

  sig = PCIB_DEVSIG_MAKE(bus,slot,fun);
  pcib_conf_read16(sig, offset, val);
  return PCIBIOS_SUCCESSFUL;
}

static int
BSP_pci_read_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char fun,
  unsigned char offset,
  uint32_t     *val
)
{
  int sig;

  sig = PCIB_DEVSIG_MAKE(bus,slot,fun);
  pcib_conf_read32(sig, offset, val);
  return PCIBIOS_SUCCESSFUL;
}

static int
BSP_pci_write_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char fun,
  unsigned char offset,
  unsigned char val
)
{
  int sig;

  sig = PCIB_DEVSIG_MAKE(bus,slot,fun);
  pcib_conf_write8(sig, offset, val);
  return PCIBIOS_SUCCESSFUL;
}

static int
BSP_pci_write_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char fun,
  unsigned char offset,
  unsigned short val
)
{
  int sig;

  sig = PCIB_DEVSIG_MAKE(bus,slot,fun);
  pcib_conf_write16(sig, offset, val);
  return PCIBIOS_SUCCESSFUL;
}

static int
BSP_pci_write_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char fun,
  unsigned char offset,
  uint32_t      val
)
{
  int sig;

  sig = PCIB_DEVSIG_MAKE(bus,slot,fun);
  pcib_conf_write32(sig, offset, val);
  return PCIBIOS_SUCCESSFUL;
}

static const pci_config_access_functions pci_bios_indirect_functions = {
  BSP_pci_read_config_byte,
  BSP_pci_read_config_word,
  BSP_pci_read_config_dword,
  BSP_pci_write_config_byte,
  BSP_pci_write_config_word,
  BSP_pci_write_config_dword
};
